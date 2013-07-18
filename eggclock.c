#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <stdio.h>

#define UPDATE 10

long epoch_when_started = -1;
long time_left = 10;
int is_paused = 0;

Font *stopwatch_face_font = nil;



void toggle_pause(void);



void
handle_kbd(int *quit){
	int rune = ekbd();
	switch(rune){
		case 'q':
			*quit = 1;
			break;
		case ' ':
			toggle_pause();
			break;
		default:
			break;
	}
}

int
how_many_digits(int num){
	int i = 1;
	while((num = num/10) != 0) i++;
	return i;
}

char*
format_time(long diff){
	long hours, mins, secs;

	hours = diff / 3600;
	mins = (diff % 3600) / 60;
	secs = diff % 60;
	
	int n = how_many_digits(hours) + 7;	

	char *str = malloc(n);

	snprintf(str, n, "%i:%02i:%02i", hours, mins, secs);

	return str;
}

long
eggclock_time(void){
	long diff = 0;
	if(!is_paused){
		diff = time(nil) - epoch_when_started;
	}
	return time_left - diff;
}

void
printtime(void){
	Point p;
	p.x = 5;
	p.y = 5;

	long t = eggclock_time();
	char *time_str = format_time(t);

	string(screen, addpt(screen->r.min, p), display->black, ZP, stopwatch_face_font, time_str);

	free(time_str);
}

void
clear_screen(void){
	draw(screen, screen->r, display->white, nil, ZP);
}

void
usage(char *s){
	printf("Usage: %s <timeout in sec>\n", s);
	exits("Usage");
}

void
main(int argc, char *argv[]){
	if(argc != 2) 
		usage(argv[0]);
	if(initdraw(0, 0, "Testing") < 0) 
		sysfatal("initdraw");
	einit(Emouse | Ekeyboard);

	stopwatch_face_font = openfont(display, "/lib/font/bit/dejavusans/dejavusans.18.font");
	if(stopwatch_face_font == nil) 
		sysfatal("openfont: Could not load font");	

	epoch_when_started = time(nil);
	time_left = atol(argv[1]);

	int quit = 0;
	char *exit_msg = nil;
	while(!quit){
		if(ecanmouse())  emouse(); 
		if(ecankbd())  handle_kbd(&quit); 

		if(eggclock_time() <= 0){
			quit = 1;
			exit_msg = "alarm";
		}

		clear_screen();

		printtime();
		
		flushimage(display, 1);
		sleep(UPDATE);
	}

	/* Cleanup */
	freefont(stopwatch_face_font);

	exits(exit_msg);
}

void
eresized(int new){
	if(new && getwindow(display, Refmesg) < 0)
		fprint(2, "can't reattach to window");
}

void
toggle_pause(void){
	if(is_paused){
		epoch_when_started = time(nil);

	}else{
		long curr_time = time(nil);
		time_left -= curr_time - epoch_when_started;
	}
	is_paused = !is_paused;
}