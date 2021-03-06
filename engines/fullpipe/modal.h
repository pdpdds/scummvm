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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef FULLPIPE_MODAL_H
#define FULLPIPE_MODAL_H

namespace Fullpipe {

class PictureObject;
class Picture;
class Sound;

struct FileInfo {
	char filename[260];
	bool empty;
	char date[16];
	int fx1;
	int fx2;
	int fy1;
	int fy2;
};

class BaseModalObject {
 public:

	BaseModalObject *_parentObj;
	ObjType _objtype;

 public:
	BaseModalObject() : _parentObj(0) { _objtype = kObjTypeDefault; }
	virtual ~BaseModalObject() {}


	virtual bool pollEvent() = 0;
	virtual bool handleMessage(ExCommand *message) = 0;
	virtual bool init(int counterdiff) = 0;
	virtual void update() = 0;

	virtual void saveload() = 0;
};

class ModalIntro : public BaseModalObject {
	int _field_8;
	int _introFlags;
	int _countDown;
	int _stillRunning;
	int _sfxVolume;

 public:
	ModalIntro();
	virtual ~ModalIntro();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}

	void finish();
};

class ModalVideoPlayer : public BaseModalObject {
public:

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message) { return true; }
	virtual bool init(int counterdiff) { return false; }
	virtual void update() {}
	virtual void saveload() {}

	void play(const char *fname);
};

class ModalMap : public BaseModalObject {
	Scene *_mapScene;
	PictureObject *_pic;
	bool _isRunning;
	Common::Rect _rect1;
	int _x;
	int _y;
	int _flag;
	int _mouseX;
	int _mouseY;
	int _field_38;
	int _field_3C;
	int _field_40;
	Common::Rect _rect2;

 public:
	ModalMap();
	virtual ~ModalMap();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}

	void initMap();
	PictureObject *getScenePicture();
};

class ModalFinal : public BaseModalObject {
	int _flags;
	int _counter;
	int _sfxVolume;

 public:
	ModalFinal();
	virtual ~ModalFinal();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}

	void unloadScenes();
};

class ModalCredits : public BaseModalObject {
	Scene *_sceneTitles;
	PictureObject *_creditsPic;
	bool _fadeIn;
	bool _fadeOut;
	int _countdown;
	int _sfxVolume;
	int _currX;
	int _currY;
	int _maxY;

 public:
	ModalCredits();
	virtual ~ModalCredits();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}
};

struct MenuArea {
	int picIdL;
	PictureObject *picObjD;
	PictureObject *picObjL;
};

class ModalMainMenu : public BaseModalObject {
public:
	Scene *_scene;
	int _hoverAreaId;
	Common::Array<MenuArea *> _areas;
	int _menuSliderIdx;
	int _musicSliderIdx;
	MenuArea *_lastArea;
	int _sliderOffset;
	int _mfield_34;
	Common::Rect _screct;
	int _bgX;
	int _bgY;
	int _debugKeyCount;

public:
	ModalMainMenu();
	virtual ~ModalMainMenu() {}

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}

private:
	bool isSaveAllowed();
	void enableDebugMenuButton();
	void setSliderPos();
	void enableDebugMenu(char c);
	int checkHover(Common::Point &point);
	void updateVolume();
	void updateSoundVolume(Sound *snd);
	void updateSliderPos();
	bool isOverArea(PictureObject *obj, Common::Point *point);
};

class ModalHelp : public BaseModalObject {
public:
	Scene *_mainMenuScene;
	Picture *_bg;
	bool _isRunning;
	Common::Rect _rect;
	int _hx;
	int _hy;

public:
	ModalHelp();
	virtual ~ModalHelp();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}

	void launch();
};

class ModalQuery : public BaseModalObject {
public:
	ModalQuery();
	virtual ~ModalQuery();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload() {}

	bool create(Scene *sc, PictureObject *picObjList, int picId);
	int getQueryResult() { return _queryResult; }


private:
	PictureObject *_picObjList;
	PictureObject *_bg;
	PictureObject *_okBtn;
	PictureObject *_cancelBtn;
	int _queryResult;

};

class ModalSaveGame : public BaseModalObject {
public:
	ModalSaveGame();
	virtual ~ModalSaveGame();

	virtual bool pollEvent() { return true; }
	virtual bool handleMessage(ExCommand *message);
	virtual bool init(int counterdiff);
	virtual void update();
	virtual void saveload();

	void processMouse(int x, int y);

	void setScene(Scene *sc);
	void setup(Scene *sc, int mode);
	void processKey(int key);

	char *getSaveName();
	bool getFileInfo(int slot, FileInfo *fileinfo);

	Common::Rect _rect;
	int _oldBgX;
	int _oldBgY;
	PictureObject *_bgr;
	PictureObject *_okD;
	PictureObject *_okL;
	PictureObject *_cancelD;
	PictureObject *_cancelL;
	PictureObject *_emptyD;
	PictureObject *_emptyL;
	PictureObject *_fullD;
	PictureObject *_fullL;
	Scene *_menuScene;
	int _mode;
	ModalQuery *_queryDlg;
	Common::Array <FileInfo *> _files;
	Common::Array <PictureObject *> _arrayL;
	Common::Array <PictureObject *> _arrayD;
	int _queryRes;
};


} // End of namespace Fullpipe

#endif /* FULLPIPE_MODAL_H */
