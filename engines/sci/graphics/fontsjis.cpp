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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "sci/sci.h"
#include "sci/engine/state.h"
#include "sci/graphics/screen.h"
#include "sci/graphics/font.h"
#include "sci/graphics/fontsjis.h"
#include "graphics/sjis.h"

namespace Sci {

//20140521
GfxFontSjis::GfxFontSjis(GfxScreen *screen, GuiResourceId resourceId)
	: _resourceId(resourceId), _screen(screen) {
	assert(resourceId != -1);

	//if (!_screen->getUpscaledHires())
	//	error("I don't want to initialize, when not being in upscaled hires mode");

	_commonFont = Graphics::FontSJIS::createFont(Common::kPlatformPC98);

	if (!_commonFont)
		error("Could not load ScummVM's 'KOREAN.FNT'");
}

GfxFontSjis::~GfxFontSjis() {
}

GuiResourceId GfxFontSjis::getResourceId() {
	return _resourceId;
}

// Returns true for first byte of double byte characters
bool GfxFontSjis::isDoubleByte(uint16 chr) {
	//if (((chr >= 0x81) && (chr <= 0x9F)) || ((chr >= 0xE0) && (chr <= 0xEF)))
	//if(chr >= 0xac && chr <= 0xdf)

	if(chr > 0x7F)
		return true;
	return false;
}

// We can do >>1, because returned char width/height is 8 or 16 exclusively. Font returns hires size, we need lowres
byte GfxFontSjis::getHeight() {
	if(g_sci->getGameId() == GID_GK1)
		return _commonFont->getFontHeight() ;

	return _commonFont->getFontHeight() >> 1;
}

byte GfxFontSjis::getCharWidth(uint16 chr) {
	if(g_sci->getGameId() == GID_GK1)
		return _commonFont->getCharWidth(chr) ;

	return _commonFont->getCharWidth(chr) >> 1;
}
void GfxFontSjis::draw(uint16 chr, int16 top, int16 left, byte color, bool greyedOutput) {
	// TODO: Check, if character fits on screen - if it doesn't we need to skip it
	//  Normally SCI cuts the character and draws the part that fits, but the common SJIS doesn't support that

	// According to LordHoto the PC98 ROM actually split the screen into 40x25 kanji chars and was only able to place
	//  them there. So removing the lower 4 bits of the horizontal coordinate puts them into the correct position (it seems).
	//  If we don't do this, the characters will be slightly to the right, caused by "GetLongest()" inside GfxText16 that
	//  leaves the last character that is causing a split into a new line within the current line instead of removing it.
	//  That way the result will actually be too long (not our fault, sierra sci does it the same way)
	_screen->putKanjiChar(_commonFont, left & 0xFFC, top, chr, color);
}

void GfxFontSjis::drawToBuffer(uint16 chr, int16 top, int16 left, byte color, bool greyedOutput, byte *buffer, int16 bufWidth, int16 bufHeight) {
	int charWidth = MIN<int>(getCharWidth(chr), bufWidth - left);
	int charHeight = MIN<int>(getHeight(), bufHeight - top);
	byte b = 0, mask = 0xFF;
	int16 greyedTop = top;

	const uint8 fB = chr & 0x00FF;
	const uint8 sB = chr >> 8;

	//charWidth *= 2;
	//charHeight *= 2;

	byte *displayPtr = buffer + top * bufWidth + left;
	// we don't use outline, so color 0 is actually not used
	//((Graphics::FontSjisSVM*)_commonFont)->drawChar(displayPtr, chr, 5, 1, color, 0, -1, -1);

	//((Graphics::FontSjisSVM*)_commonFont)->drawChar(buffer, chr,);
	//const byte *pIn = ((Graphics::FontSjisSVM*)_commonFont)->drawChar(chr);

	if(isDoubleByte(chr))
	{
		double scaleWidth =  (double)8 / (double)16;
		double scaleHeight = (double)8 / (double)16;

		byte *pIn = (byte*)((Graphics::FontKorSVM*)_commonFont)->getCharData(chr);
		for (int i = 0; i < _commonFont->getFontHeight(); i++) {
			if (greyedOutput)
				mask = ((greyedTop++) % 2) ? 0xAA : 0x55;
			for (int done = 0; done < _commonFont->getCharWidth(chr); done++) {
				if ((done & 7) == 0) // fetching next data byte
					b = *(pIn++) & mask;
				if (b & 0x80) {	// if MSB is set - paint it
					int offset = (top + i) * bufWidth + (left + done);

					if(offset < bufWidth * bufHeight)
					buffer[offset] = color;
				}
				b = b << 1;
			}
		}

		/*for(int cy = 0; cy < 8; cy++)
		{
			for(int cx = 0; cx < 8; cx++)
			{
				//if (greyedOutput)
				//	mask = ((greyedTop++) % 2) ? 0xAA : 0x55;

				int offset = (top + cy) * bufWidth + (left + cx);
				int nearestMatch =  (((int)(cy / scaleHeight) * (16)) + ((int)(cx / scaleWidth) ) );
				int div = nearestMatch % 8;
				b = (*(pIn + (byte)(nearestMatch / 8)));//  & mask;;

				while(div > 0)
				{
					b = b << 1;
					div--;
				}
				if (b & 0x80) 			
					buffer[offset] =  color;				
			}
		}*/
	}
	else
	{
		byte *pIn = (byte*)((Graphics::FontKorSVM*)_commonFont)->getCharData(chr);
		for (int i = 0; i < charHeight; i++) {
			if (greyedOutput)
				mask = ((greyedTop++) % 2) ? 0xAA : 0x55;
			for (int done = 0; done < charWidth << 1; done++) {
				if ((done & 7) == 0) // fetching next data byte
					b = *(pIn++) & mask;
				if (b & 0x80) {	// if MSB is set - paint it
					int offset = (top + i) * bufWidth + (left + done);
					buffer[offset] = color;
				}
				b = b << 1;
			}
		}

	}
	

	/*static char buf[65536];
	memset(buf, 0, 65536);


	byte *pIn = (byte*)((Graphics::FontKorSVM*)_commonFont)->getCharData(chr);
	for (int i = 0; i < charHeight; i++) {
		if (greyedOutput)
			mask = ((greyedTop++) % 2) ? 0xAA : 0x55;
		for (int done = 0; done < charWidth; done++) {
			if ((done & 7) == 0) // fetching next data byte
				b = *(pIn++) & mask;
			if (b & 0x80) {	// if MSB is set - paint it
				int offset = i * charWidth + (done);
				buf[offset] = color;
			}
			b = b << 1;
		}
	}

	for(int y = 0; y < charHeight; y+=2)
	{
		for (int x = 0; x < charWidth; x+=2) 
		{
			int count = 0;
			if(buf[y * charWidth + x] == color)
				count++;
			if(buf[y * charWidth + x+1] == color)
				count++;
			if(buf[(y+1) * charWidth + x] == color)
				count++;
			if(buf[(y+1) * charWidth + x+1] == color)
				count++;

			if(count >= 1)
			{
				int offset = (top + y/2) * bufWidth + (left + x/2);
				buffer[offset] = color;
			}
		
		}
	}*/
}

} // End of namespace Sci
