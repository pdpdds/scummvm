/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "kyra/kyra.h"
#include "kyra/timer.h"

#include "common/func.h"
#include "common/savefile.h"

namespace Kyra {

namespace {
struct TimerResync : public Common::UnaryFunction<TimerEntry&, void> {
	uint32 _tickLength, _curTime;
	TimerResync(KyraEngine *vm, uint32 curTime) : _tickLength(vm->tickLength()), _curTime(curTime) {}

	void operator()(TimerEntry &entry) const {
		if (entry.lastUpdate < 0) {
			if ((entry.lastUpdate + _curTime) <= 0)
				entry.nextRun = 0;
			else
				entry.nextRun = _curTime + entry.lastUpdate + entry.countdown * _tickLength;
		} else {
			uint32 nextRun = entry.lastUpdate + entry.countdown * _tickLength;
			if (_curTime < nextRun)
				nextRun = 0;
			entry.nextRun = nextRun;
		}
	}
};

struct TimerEqual : public Common::UnaryFunction<const TimerEntry&, bool> {
	uint8 _id;
	
	TimerEqual(uint8 id) : _id(id) {}

	bool operator()(const TimerEntry &entry) const {
		return entry.id == _id;
	}
};
} // end of anonymous namespace

void TimerManager::reset() {
	for (Iterator pos = _timers.begin(); pos != _timers.end(); ++pos) {
		delete pos->func;
	}

	_timers.clear();
}

void TimerManager::addTimer(uint8 id, TimerFunc *func, int countdown, bool enabled) {
	debugC(9, kDebugLevelTimer, "TimerManager::addTimer(%d, %p, %d, %d)", id, (const void*)func, countdown, enabled);

	Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end()) {
		warning("Adding allready existing timer %d", id);
		return;
	}

	TimerEntry newTimer;
	
	newTimer.id = id;
	newTimer.countdown = countdown;
	newTimer.enabled = enabled ? 1 : 0;
	newTimer.lastUpdate = newTimer.nextRun = 0;
	newTimer.func = func;
	
	_timers.push_back(newTimer);
}

void TimerManager::update() {
	debugC(9, kDebugLevelTimer, "TimerManager::update()");

	if (_system->getMillis() < _nextRun)
		return;
	
	_nextRun += 99999;

	for (Iterator pos = _timers.begin(); pos != _timers.end(); ++pos) {
		if (pos->enabled && pos->countdown >= 0 && pos->nextRun <= _system->getMillis()) {
			if (pos->func && *pos->func)
				(*pos->func)(pos->id);

			uint32 curTime = _system->getMillis();
			pos->lastUpdate = curTime;
			pos->nextRun = curTime + pos->countdown * _vm->tickLength();
			
			_nextRun = MIN(_nextRun, pos->nextRun);
		}
	}
}

void TimerManager::resync() {
	debugC(9, kDebugLevelTimer, "TimerManager::resync()");

	_nextRun = 0;	// force rerun
	Common::for_each(_timers.begin(), _timers.end(), TimerResync(_vm, _system->getMillis()));	
}

void TimerManager::resetNextRun() {
	debugC(9, kDebugLevelTimer, "TimerManager::resetNextRun()");
	_nextRun = 0;
}

void TimerManager::setCountdown(uint8 id, int32 countdown) {
	debugC(9, kDebugLevelTimer, "TimerManager::setCountdown(%d, %d)", id, countdown);

	Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end()) {
		timer->countdown = countdown;

		if (countdown >= 0) {
			uint32 curTime = _system->getMillis();
			timer->lastUpdate = curTime;
			timer->nextRun = curTime + countdown * _vm->tickLength();
		
			_nextRun = MIN(_nextRun, timer->nextRun);
		}
	} else {
		warning("TimerManager::setCountdown: No timer %d", id);
	}
}

void TimerManager::setDelay(uint8 id, int32 countdown) {
	debugC(9, kDebugLevelTimer, "TimerManager::setDelay(%d, %d)", id, countdown);

	Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end())
		timer->countdown = countdown;
	else
		warning("TimerManager::setDelay: No timer %d", id);
}

int32 TimerManager::getDelay(uint8 id) const {
	debugC(9, kDebugLevelTimer, "TimerManager::getDelay(%d)", id);

	CIterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end())
		return timer->countdown;

	warning("TimerManager::getDelay: No timer %d", id);
	return -1;
}

bool TimerManager::isEnabled(uint8 id) const {
	debugC(9, kDebugLevelTimer, "TimerManager::isEnabled(%d)", id);

	CIterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end())
		return (timer->enabled == 1);

	warning("TimerManager::isEnabled: No timer %d", id);
	return false;
}

void TimerManager::enable(uint8 id) {
	debugC(9, kDebugLevelTimer, "TimerManager::enable(%d)", id);

	Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end())
		timer->enabled = 1;
	else
		warning("TimerManager::enable: No timer %d", id);
}

void TimerManager::disable(uint8 id) {
	debugC(9, kDebugLevelTimer, "TimerManager::disable(%d)", id);

	Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
	if (timer != _timers.end())
		timer->enabled = 0;
	else
		warning("TimerManager::disable: No timer %d", id);
}

void TimerManager::loadDataFromFile(Common::InSaveFile *file, int version) {
	debugC(9, kDebugLevelTimer, "TimerManager::loadDataFromFile(%p, %d)", (const void*)file, version);
	
	if (version <= 7) {
		_nextRun = 0;
		for (int i = 0; i < 32; ++i) {
			uint8 enabled = file->readByte();
			int32 countdown = file->readSint32BE();
			uint32 nextRun = file->readUint32BE();
			
			Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(i));
			if (timer != _timers.end()) {
				timer->enabled = enabled;
				timer->countdown = countdown;

				if (nextRun) {
					timer->nextRun = nextRun + _system->getMillis();
					timer->lastUpdate = timer->nextRun - countdown * _vm->tickLength();
				} else {
					uint32 curTime = _system->getMillis();
					timer->nextRun = curTime;
					timer->lastUpdate = curTime - countdown * _vm->tickLength();
				}
			} else {
				warning("Loading timer data for non existing timer %d", i);
			}			
		}
	} else {
		int entries = file->readByte();
		for (int i = 0; i < entries; ++i) {
			uint8 id = file->readByte();

			Iterator timer = Common::find_if(_timers.begin(), _timers.end(), TimerEqual(id));
			if (timer != _timers.end()) {
				timer->enabled = file->readByte();
				timer->countdown = file->readSint32BE();
				timer->lastUpdate = file->readSint32BE();
			} else {
				warning("Loading timer data for non existing timer %d", id);
				file->seek(7, SEEK_CUR);
			}
		}
		
		resync();
	}
}

void TimerManager::saveDataToFile(Common::OutSaveFile *file) const {
	debugC(9, kDebugLevelTimer, "TimerManager::saveDataToFile(%p)", (const void*)file);
	
	file->writeByte(count());
	for (CIterator pos = _timers.begin(); pos != _timers.end(); ++pos) {
		file->writeByte(pos->id);
		file->writeByte(pos->enabled);
		file->writeSint32BE(pos->countdown);
		file->writeSint32BE(pos->lastUpdate - _system->getMillis());
	}
}

} // end of namespace Kyra
