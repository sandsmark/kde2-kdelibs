/**************************************************************************

    midiout.cc   - class midiOut which handles the /dev/sequencer device
    Copyright (C) 1997  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include "midiout.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "sndcard.h"
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <sys/param.h>

//SEQ_DEFINEBUF (1024); 

SEQ_USE_EXTBUF();

//#define MIDIOUTDEBUG

midiOut::midiOut(int d)
{
seqfd = -1;
device= d;
count=0.0;
lastcount=0.0;
Map=new MidiMapper(NULL);
rate=100;
convertrate=10;
ok=1;
};

midiOut::~midiOut()
{
delete Map;
closeDev();
};

void midiOut::openDev (int sqfd)
{
ok=1;
//seqfd = open("/dev/sequencer", O_WRONLY, 0);
seqfd=sqfd;
if (seqfd==-1)
    {
    printf("ERROR: Could not open /dev/sequencer\n");
    ok=0;
    return;
    };
ioctl(seqfd,SNDCTL_SEQ_NRSYNTHS,&ndevs);
ioctl(seqfd,SNDCTL_SEQ_NRMIDIS,&nmidiports);
rate=0;
int r=ioctl(seqfd,SNDCTL_SEQ_CTRLRATE,&rate);
if ((r==-1)||(rate<=0)) rate=HZ;

midi_info midiinfo;
midiinfo.device=device;
if (ioctl(seqfd,SNDCTL_MIDI_INFO,&midiinfo)!=-1)
	{
	if (strcmp(midiinfo.name,"GUS MIDI emulation")==0) 
			rate=(int)(((double)rate)*2.5);
	};

convertrate=1000/rate;

#ifdef MIDIOUTDEBUG
printf("Number of synth devices : %d\n",ndevs);
printf("Number of midi ports : %d\n",nmidiports);
printf("Rate : %d\n",rate);

int i;
synth_info synthinfo;
for (i=0;i<ndevs;i++)
    {
    synthinfo.device=i;
    if (ioctl(seqfd,SNDCTL_SYNTH_INFO,&synthinfo)!=-1)
	{
	printf("----");
        printf("Device : %d\n",i);
	printf("Name : %s\n",synthinfo.name);
	switch (synthinfo.synth_type)
	    {
	    case (SYNTH_TYPE_FM) : printf("FM\n");break;
	    case (SYNTH_TYPE_SAMPLE) : printf("Sample\n");break;
	    case (SYNTH_TYPE_MIDI) : printf("Midi\n");break;
	    default : printf("default type\n");break;
	    };
	switch (synthinfo.synth_subtype)
	    {
	    case (FM_TYPE_ADLIB) : printf("Adlib\n");break;
	    case (FM_TYPE_OPL3) : printf("Opl3\n");break;
	    case (MIDI_TYPE_MPU401) : printf("Mpu-401\n");break;
	    case (SAMPLE_TYPE_GUS) : printf("Gus\n");break;
	    default : printf("default subtype\n");break;
	    };
	};
    };

for (i=0;i<nmidiports;i++)
    {
    midiinfo.device=i;
    if (ioctl(seqfd,SNDCTL_MIDI_INFO,&midiinfo)!=-1)
	{
	printf("----");
        printf("Device : %d\n",i);
	printf("Name : %s\n",midiinfo.name);
	printf("Device type : %d\n",midiinfo.dev_type);
	};
    };

#endif


count=0.0;
lastcount=0.0;
if (nmidiports<=0)
    {
    printf("ERROR: There is no midi port !!\n");
    ok=0;
    return;
    };

};

void midiOut::closeDev (void)
{
if (!OK()) return;
SEQ_STOP_TIMER();
SEQ_DUMPBUF();
//if (seqfd>=0)
//    close(seqfd);
seqfd=-1;
printf("Device %d closed\n",device);
};

void midiOut::initDev (void)
{
int chn;
if (!OK()) return;
count=0.0;
lastcount=0.0;
uchar gm_reset[5]={0x7e, 0x7f, 0x09, 0x01, 0xf7};
sysex(gm_reset, sizeof(gm_reset));
for (chn=0;chn<16;chn++)
    {
    chn_mute[chn]=0;
    chnPatchChange(chn,0);
    chnPressure(chn,127);
    chnPitchBender(chn, 0x00, 0x40);
    chnController(chn, CTL_MAIN_VOLUME,127);
    chnController(chn, CTL_EXT_EFF_DEPTH, 0);
    chnController(chn, CTL_CHORUS_DEPTH, 0);
    chnController(chn, 0x4a, 127);

    };
printf("Device %d initialized\n",device);
};

void midiOut::useMapper(MidiMapper *map)
{
delete Map;
Map=map;
};

void midiOut::noteOn  (uchar chn, uchar note, uchar vel)
{
if (vel==0)
    {
    noteOff(chn,note,vel);
    }
   else
    {
    SEQ_MIDIOUT(device, MIDI_NOTEON + Map->Channel(chn));
    SEQ_MIDIOUT(device, Map->Key(chn,chn_patch[chn],note));
    SEQ_MIDIOUT(device, vel);
    };
#ifdef MIDIOUTDEBUG
printf("Note ON >\t chn : %d\tnote : %d\tvel: %d\n",chn,note,vel);
#endif
};

void midiOut::noteOff (uchar chn, uchar note, uchar vel)
{
SEQ_MIDIOUT(device, MIDI_NOTEOFF + Map->Channel(chn));
SEQ_MIDIOUT(device, Map->Key(chn,chn_patch[chn],note));
SEQ_MIDIOUT(device, vel);
#ifdef MIDIOUTDEBUG
printf("Note OFF >\t chn : %d\tnote : %d\tvel: %d\n",chn,note,vel);
#endif
};

void midiOut::keyPressure (uchar chn, uchar note, uchar vel)
{
SEQ_MIDIOUT(device, MIDI_KEY_PRESSURE + Map->Channel(chn));
SEQ_MIDIOUT(device, Map->Key(chn,chn_patch[chn],note));
SEQ_MIDIOUT(device, vel);
};

void midiOut::chnPatchChange (uchar chn, uchar patch)
{
#ifdef MIDIOUTDEBUG
printf("PATCHCHANGE [%d->%d] %d -> %d\n",chn,Map->Channel(chn),patch,Map->Patch(chn,patch));
#endif
SEQ_MIDIOUT(device, MIDI_PGM_CHANGE + Map->Channel(chn));
SEQ_MIDIOUT(device, Map->Patch(chn,patch));
chn_patch[chn]=patch;
};

void midiOut::chnPressure (uchar chn, uchar vel)
{
SEQ_MIDIOUT(device, MIDI_CHN_PRESSURE + Map->Channel(chn));
SEQ_MIDIOUT(device, vel);

chn_pressure[chn]=vel;
};

void midiOut::chnPitchBender(uchar chn,uchar lsb, uchar msb)
{
SEQ_MIDIOUT(device, MIDI_PITCH_BEND + Map->Channel(chn));
SEQ_MIDIOUT(device, lsb);
SEQ_MIDIOUT(device, msb);
chn_bender[chn]=(msb << 8) | (lsb & 0xFF);
};

void midiOut::chnController (uchar chn, uchar ctl, uchar v) 
{
SEQ_MIDIOUT(device, MIDI_CTL_CHANGE + Map->Channel(chn));
SEQ_MIDIOUT(device, ctl);
SEQ_MIDIOUT(device, v);

chn_controller[chn][ctl]=v;
};

void midiOut::sysex(uchar *data, ulong size)
{
ulong i=0;
SEQ_MIDIOUT(device, MIDI_SYSTEM_PREFIX);
while (i<size)
    {
    SEQ_MIDIOUT(device, *data);
    data++;
    i++;
    };
#ifdef MIDIOUTDEBUG
printf("sysex\n");
#endif
};

void midiOut::channelSilence (uchar chn)
{
uchar i;
for ( i=0; i<127; i++)
    {
    noteOff(chn,i,0);
    };
SEQ_DUMPBUF();
};

void midiOut::channelMute(uchar chn, int a)
{
if (a==1)
    {
    chn_mute[chn]=a;
    channelSilence(chn);
    }
  else if (a==0)
    {
    chn_mute[chn]=a;
    };
/*  else ignore the call to this procedure */
};

void midiOut::seqbuf_dump (void)
{
    if (_seqbufptr)
        if (write (seqfd, _seqbuf, _seqbufptr) == -1)
        {
            perror ("write /dev/sequencer");
            exit (-1);
        }
    _seqbufptr = 0;
};

void midiOut::seqbuf_clean(void)
{
_seqbufptr=0;
};

void midiOut::wait(double ticks)
{
SEQ_WAIT_TIME(((int)(ticks/convertrate)));
#ifdef MIDIOUTDEBUG
printf("Wait  >\t ticks: %g\n",ticks);
#endif
};

void midiOut::tmrSetTempo(int v)
{
#ifdef MIDIOUTDEBUG
printf("SETTEMPO  >\t tempo: %d\n",v);
#endif

SEQ_SET_TEMPO(v);
//SEQ_DUMPBUF();
};

void midiOut::sync(int i)
{
#ifdef MIDIOUTDEBUG
printf("Sync %d\n",i);
#endif
if (i==1) 
    {    
    seqbuf_clean();
/* If you have any problem, try removing the next 2 lines, 
	I though they would be useful here, but I don't know
	what they do :-) */
    ioctl(seqfd,SNDCTL_SEQ_RESET);
    ioctl(seqfd,SNDCTL_SEQ_PANIC);
    };
ioctl(seqfd, SNDCTL_SEQ_SYNC);
};

void midiOut::tmrStart(void)
{
SEQ_START_TIMER();
SEQ_DUMPBUF();
};

void midiOut::tmrStop(void)
{
SEQ_STOP_TIMER();
SEQ_DUMPBUF();
};

void midiOut::tmrContinue(void)
{
SEQ_CONTINUE_TIMER();
SEQ_DUMPBUF();
};

char *midiOut::getMidiMapFilename(void)
{
return (Map!=NULL) ? Map->getFilename() : (char *)"";
};
