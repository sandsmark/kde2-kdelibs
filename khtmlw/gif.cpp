/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
              (C) 1997 Torben Weis (weis@kde.org)
			  (C) 1997 Sirtaj Singh Kang (taj@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
////////////////////////////////////////////////////
//
// Transparent support for GIF files in Qt Pixmaps,
// using GIFLIB library.
//
// Much of this code is duplicated from gif2x11.c, which is distributed
// with GIFLIB.
//
// Sirtaj Kang, Oct 1996.
//
// Current Problems:
// * No GIF extension blocks supported.
// * Lousy error reporting.
// * Buffers entire image in mem before moving to QImage
// * Skipped IODevice completely. How to get around this?
//
// GIF write support and transparency support added by Richard J. Moore
// Note that Qt will support gif files internally in version 1.3 so this
// code is not worth spending much time on.
// moorer@cs.man.ac.uk
//
// $Id$


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <qimage.h>

#include "gif.h"

extern "C" {
#include <gif_lib.h>
};

void read_gif_file(QImageIO * imageio)
{
  int i, j, Size, Row, Col, Width, Height, ExtCode, Count;
  GifRecordType RecordType;
  GifByteType *Extension;
  GifRowType *ScreenBuffer;
  GifFileType *GifFile;

  QImage image;
  ColorMapObject *Colourmap;
  int trans= -1; // Index of the transparant colour, -1 if no transparent colour
//  unsigned int *ui_row;
  unsigned char *uc_row;
  int InterlacedOffset[] =
  {0, 4, 2, 1},		/* The way Interlaced image should. */
    InterlacedJumps[] =
    {8, 8, 4, 2};		/* be read - offsets and jumps... */


    // Initialise GIF struct and read init block

    if ((GifFile = DGifOpenFileName(imageio->fileName())) == 0L)
      return;

    // Create the new image using read dimensions.
    // Currently hardcoded to 8 bit.

    image.create(GifFile->SWidth, GifFile->SHeight, 8,
		 GifFile->SColorMap->ColorCount,
		 QImage::BigEndian);

    /* Allocate the screen as vector of column of rows. We cannt allocate    */
    /* the all screen at once, as this broken minded CPU can allocate up to  */
    /* 64k at a time and our image can be bigger than that:                  */
    /* Note this screen is device independent - its the screen as defined by */
    /* the GIF file parameters itself.                                       */

    if ((ScreenBuffer = (GifRowType *)
	 malloc(GifFile->SHeight * sizeof(GifRowType *))) == 0L)
      return;

    Size = GifFile->SWidth * sizeof(GifPixelType);	/* Bytes in one row */
    if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == 0L)	/* First row */
      return;

    for (i = 0; i < GifFile->SWidth; i++)	/* Set color to BackGround */
      ScreenBuffer[0][i] = GifFile->SBackGroundColor;

    for (i = 1; i < GifFile->SHeight; i++) {
      /* Allocate the other rows, and set their color to background too: */
      if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == 0L)
	return;

      memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
    }

    /* Scan the content of the GIF file and load the image(s) in: */

    do {
      if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
	PrintGifError();
	return;
      }
      switch (RecordType) {
      case IMAGE_DESC_RECORD_TYPE:
	if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
	  PrintGifError();
	  return;
	}
	Row = GifFile->Image.Top;	/* Image Position relative to Screen. */
	Col = GifFile->Image.Left;
	Width = GifFile->Image.Width;
	Height = GifFile->Image.Height;
	if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
	    GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
	  fprintf(stderr, "Image is not confined to screen dimension, aborted.\n");
	  return;
	}
	if (GifFile->Image.Interlace) {
	  /* Need to perform 4 passes on the images: */
	  for (Count = i = 0; i < 4; i++)
	    for (j = Row + InterlacedOffset[i]; j < Row + Height;
		 j += InterlacedJumps[i]) {
	      if (DGifGetLine(GifFile, &ScreenBuffer[j][Col],
			      Width) == GIF_ERROR) {
		PrintGifError();
		return;
	      }
	    }
	} else {
	  for (i = 0; i < Height; i++) {
	    if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col],
			    Width) == GIF_ERROR) {
	      PrintGifError();
	      return;
	    }
	  }
	}
	break;
      case EXTENSION_RECORD_TYPE:
	/* Skip any extension blocks in file: */
	if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
	  PrintGifError();
	  return;
	}
	if ( ExtCode == 249 )	// Graphic Control Ext
	  {
#ifdef KPDEBUG
	    fprintf(stderr, "Found graphic control extension\n");
#endif
	    // extract tranparent pixel stuff
	    if (Extension[1] & 1) {
	      trans= Extension[4];
#ifdef KPDEBUG
	      fprintf(stderr, "Set trans to %d\n", Extension[4]);
#endif
	    }
	  }
	do {
	  if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
	    PrintGifError();
	    return;
	  }
	  if (Extension != 0L) {
	    if ( ExtCode == 249 )	// Graphic Control Ext
	    {
#ifdef KPDEBUG
	    fprintf(stderr, "Found graphic control extension in next\n");
#endif
	    // extract tranparent pixel stuff
	    if (Extension[1] & 1) {
	      trans= Extension[4];
#ifdef KPDEBUG
	      fprintf(stderr, "Set trans to %d\n", Extension[4]);
#endif
	    }
	    }
	  }
	} while	 (Extension != 0L);
	break;
      case TERMINATE_RECORD_TYPE:
	break;
      default:		/* Should be traps by DGifGetRecordType. */
	break;
      }
    }
    while (RecordType != TERMINATE_RECORD_TYPE);

    /* Set QImage colour map */

    Colourmap = (GifFile->Image.ColorMap
		 ? GifFile->Image.ColorMap
		 : GifFile->SColorMap);

    for (j = 0; j < Colourmap->ColorCount; j++) {
      /* Set colour to rgb and set as opaque */
      image.setColor(j, qRgb(Colourmap->Colors[j].Red,
			     Colourmap->Colors[j].Green,
			     Colourmap->Colors[j].Blue) | 0xff000000);
    }

    /* Copy image from Row buffers */
    for (j = 0; j < GifFile->SHeight; j++) {
      uc_row = (unsigned char *) image.scanLine(j);
      for (i = 0; i < GifFile->SWidth; i++) {
	*uc_row++ = ScreenBuffer[j][i];
      }
    }
    if (trans != -1) {
      /* Make the transparent colour transparent */
      image.setColor(trans, qRgb(Colourmap->Colors[trans].Red,
			     Colourmap->Colors[trans].Green,
			     Colourmap->Colors[trans].Blue) & 0x00ffffff);
      image.setAlphaBuffer(TRUE);
    }

    /* Commit image */

    imageio->setImage(image);
    imageio->setStatus(0);

    if (DGifCloseFile(GifFile) == GIF_ERROR)
      PrintGifError();

}				/*read_gif_file */


void write_gif_file(QImageIO *imageio)
{
  int i, status;
  GifFileType *GifFile;
  ColorMapObject *screenColourmap;
  ColorMapObject *imageColourmap;

#ifdef KPDEBUG
  fprintf(stderr, "write_gif_file()\n");
#endif

  imageColourmap= MakeMapObject(256, 0L);
  if (!imageColourmap) {
    fprintf(stderr, "Could not create image colour map\n");
    return;
  }

  screenColourmap= MakeMapObject(256, 0L);
  if (!screenColourmap) {
    fprintf(stderr, "Could not create screen colour map\n");
    return;
  }

#ifdef KPDEBUG
  fprintf(stderr, "Made Colourmap size 256, image colours= %d\n",
	  imageio->image().numColors());
#endif

  for (i= 0; i < 256; i++) {
    if (i <imageio->image().numColors()) {
      imageColourmap->Colors[i].Red= qRed(imageio->image().color(i));
      imageColourmap->Colors[i].Green= qGreen(imageio->image().color(i));
      imageColourmap->Colors[i].Blue= qBlue(imageio->image().color(i));
    }
    else {
      imageColourmap->Colors[i].Red= 0;
      imageColourmap->Colors[i].Green= 0;
      imageColourmap->Colors[i].Blue= 0;
    }
  }

  for (i= 0; i < 256; i++) {
    if (i <imageio->image().numColors()) {
      screenColourmap->Colors[i].Red= qRed(imageio->image().color(i));
      screenColourmap->Colors[i].Green= qGreen(imageio->image().color(i));
      screenColourmap->Colors[i].Blue= qBlue(imageio->image().color(i));
    }
    else {
      screenColourmap->Colors[i].Red= 0;
      screenColourmap->Colors[i].Green= 0;
      screenColourmap->Colors[i].Blue= 0;
    }
  }

  QString filename = imageio->fileName();
  GifFile= EGifOpenFileName(filename.data(), 0);
  if (!GifFile) {
    FreeMapObject(imageColourmap);
    FreeMapObject(screenColourmap);
    fprintf(stderr, "Could not open file\n");
    return;
  }

#ifdef KPDEBUG
  fprintf(stderr, "Creating screen desc\n");
#endif

  status= EGifPutScreenDesc(GifFile,
			    imageio->image().width(),
			    imageio->image().height(),
			    256,
			    0,
			    screenColourmap);

  if (status != GIF_OK) {
    EGifCloseFile(GifFile);
    fprintf(stderr, "Could not create screen description\n");
    return;
  }

#ifdef KPDEBUG
  fprintf(stderr, "Creating image description\n");
#endif

  status= EGifPutImageDesc(GifFile,
			   0, 0,
			   imageio->image().width(),
			   imageio->image().height(),
			   0,
			   imageColourmap);

  if (status != GIF_OK) {
    EGifCloseFile(GifFile);
    return;
  }

#ifdef KPDEBUG
  fprintf(stderr, "Writing image data\n");
#endif

  for (i = 0; status && (i < imageio->image().height()); i++) {
    status= EGifPutLine(GifFile, 
			imageio->image().scanLine(i),
			imageio->image().bytesPerLine());
  }

  if (status != GIF_OK) {
    PrintGifError();
    EGifCloseFile(GifFile);
    return;
  }

  if (EGifCloseFile(GifFile) != GIF_OK) {
    PrintGifError();
    return;
  }

  return;
}

