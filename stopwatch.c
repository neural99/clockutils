#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <stdio.h>

#define UPDATE 10

int is_stopwatch_running = 0;

long epoch_when_stopwatch_started = -1;
long saved_time = 0;

Font *stopwatch_face_font = nil;



void reset(void);
void toggle_running(void);



void
handle_kbd(int *quit){
	int rune = ekbd();
	switch(rune){
		case 'q':
			if(!is_stopwatch_running)
				*quit = 1;
			break;
		case ' ':
			toggle_running();
			break;
		case 'r':
			reset();
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
current_stopwatch_time(void){
	long curr_time = 0;
	if(is_stopwatch_running){
		curr_time = time(nil);
	}else{
		curr_time = epoch_when_stopwatch_started;
	}

	long diff = curr_time - epoch_when_stopwatch_started;

	return diff + saved_time;
}

void
printtime(void){
	Point p;
	p.x = 5;
	p.y = 5;

	long t = current_stopwatch_time();
	char *time_str = format_time(t);

	string(screen, addpt(screen->r.min, p), display->black, ZP, stopwatch_face_font, time_str);

	free(time_str);
}

void
clear_screen(void){
	draw(screen, screen->r, display->white, nil, ZP);
}

void
main(int argc, char *argv[]){
	if(initdraw(0, 0, "Testing") < 0) 
		sysfatal("initdraw");
	einit(Emouse | Ekeyboard);

	stopwatch_face_font = openfont(display, "/lib/font/bit/dejavusans/dejavusans.18.font");
	if(stopwatch_face_font == nil) 
		sysfatal("openfont: Could not load font");	

	epoch_when_stopwatch_started = time(nil);

	int quit = 0;
	while(!quit){
		if(ecanmouse())  emouse(); 
		if(ecankbd())  handle_kbd(&quit); 

		clear_screen();

		printtime();
		
		flushimage(display, 1);
		sleep(UPDATE);
	}

	/* Cleanup */
	freefont(stopwatch_face_font);
}

void
eresized(int new){
	if(new && getwindow(display, Refmesg) < 0)
		fprint(2, "can't reattach to window");
}

void
reset(void){
	/* Log the current stopwatch time to the console */
	long t = current_stopwatch_time();
	char *time_str = format_time(t);
	printf("Reseting. Time: %s\n", time_str);
	free(time_str);

	/* Reset the actual stopwatch */
	saved_time = 0;
	epoch_when_stopwatch_started = time(nil);
}

void
toggle_running(void){
	if(is_stopwatch_running){
		long curr_time = time(nil);
		saved_time += curr_time - epoch_when_stopwatch_started;
	}else{
		epoch_when_stopwatch_started = time(nil);
	}
	is_stopwatch_running = !is_stopwatch_running;
}