#include <pebble.h>

#define KEY_SHOW_WEATHER 0
#define KEY_USE_CELSIUS 1
#define KEY_TEMPERATURE 2
#define KEY_TEMPERATURE_IN_C 3
#define KEY_CONDITIONS 4

static Window *s_main_window;
static TextLayer *s_time_layer, *s_ampm_layer, *s_date_layer, *s_temp_layer, *s_conditions_layer;
static Layer *weather_layer;
static GFont s_time_font, s_date_font, s_weather_font;
static bool show_weather = 1;
static bool use_celsius = 0;

void on_animation_stopped(Animation *anim, bool finished, void *context) {
    //Free the memory used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay) {
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
 
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
 
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
 
    //Start animation!
    animation_schedule((Animation*) anim);
}

static void update_layers() {
	if (show_weather == 0) {
		layer_set_hidden(weather_layer, true);
	} else {
		layer_set_hidden(weather_layer, false);
	}
	layer_mark_dirty(weather_layer);
}

static void animate_layers() {
	GRect start = GRect(144, 0, 144, 168);
	GRect finish = GRect(0, 0, 144, 168);

	animate_layer(weather_layer, &start, &finish, 1000, 0);
	animate_layer(weather_layer, &finish, &start, 1000, 5000);
}

static void set_text_colors(GColor color) {
	text_layer_set_text_color(s_ampm_layer, color);
	text_layer_set_text_color(s_time_layer, color);
	text_layer_set_text_color(s_date_layer, color);
	text_layer_set_text_color(s_temp_layer, color);
	text_layer_set_text_color(s_conditions_layer, color);
}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	int hour = tick_time->tm_hour;

	if(hour >= 21) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, "night");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(gcolor_legible_over(GColorBlack));
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else if(hour >= 18) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, "evening");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorOxfordBlue);
			set_text_colors(gcolor_legible_over(GColorOxfordBlue));
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else if(hour >= 15) {
		text_layer_set_text(s_ampm_layer, "late");
		text_layer_set_text(s_time_layer, "afternoon");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorSunsetOrange);
			set_text_colors(gcolor_legible_over(GColorSunsetOrange));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 12) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, "afternoon");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVividCerulean);
			set_text_colors(gcolor_legible_over(GColorVividCerulean));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 9) {
		text_layer_set_text(s_ampm_layer, "late");
		text_layer_set_text(s_time_layer, "morning");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVividCerulean);
			set_text_colors(gcolor_legible_over(GColorVividCerulean));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 6) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, "morning");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVividCerulean);
			set_text_colors(gcolor_legible_over(GColorVividCerulean));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 3) {
		text_layer_set_text(s_ampm_layer, "early");
		text_layer_set_text(s_time_layer, "morning");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVeryLightBlue);
			set_text_colors(gcolor_legible_over(GColorVeryLightBlue));	
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else if(hour >= 0) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, "midnight");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(gcolor_legible_over(GColorBlack));
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else {
		text_layer_set_text(s_ampm_layer, "error");
		text_layer_set_text(s_time_layer, "error");
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorRed);
			set_text_colors(gcolor_legible_over(GColorRed));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	}	

	static char date_buffer[] = "WWWWWWWWW";

	strftime(date_buffer, sizeof("WWWWWWWWW"), "%A", tick_time);
	text_layer_set_text(s_date_layer, date_buffer);

	// For localization
	/*if (strcmp(date_buffer, "Monday") == 0) {
		text_layer_set_text(s_date_layer, "Monday");
	} else if(strcmp(date_buffer, "Tuesday") == 0) {
		text_layer_set_text(s_date_layer, "Tuesday");
	} else if(strcmp(date_buffer, "Wednesday") == 0) {
		text_layer_set_text(s_date_layer, "Wednesday");
	} else if(strcmp(date_buffer, "Thursday") == 0) {
		text_layer_set_text(s_date_layer, "Thursday");
	} else if(strcmp(date_buffer, "Friday") == 0) {
		text_layer_set_text(s_date_layer, "Friday"));
	} else if(strcmp(date_buffer, "Saturday") == 0) {
		text_layer_set_text(s_date_layer, "Saturday");
	} else if(strcmp(date_buffer, "Sunday") == 0) {
		text_layer_set_text(s_date_layer, "Sunday");
	} else {
		text_layer_set_text(s_date_layer, date_buffer);
	}*/
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
	static char temp_buffer[25];
	static char temp_c_buffer[25];
	static char conditions_buffer[200];

	Tuple *show_weather_t = dict_find(iter, KEY_SHOW_WEATHER);
	Tuple *use_celsius_t = dict_find(iter, KEY_USE_CELSIUS);
	Tuple *temperature_t = dict_find(iter, KEY_TEMPERATURE);
	Tuple *temperature_c_t = dict_find(iter, KEY_TEMPERATURE_IN_C);
	Tuple *conditions_t = dict_find(iter, KEY_CONDITIONS);

	if (show_weather_t) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_WEATHER received!");
		show_weather = show_weather_t->value->int8;

		persist_write_int(KEY_SHOW_WEATHER, show_weather);
	}

	if (use_celsius_t) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_USE_CELSIUS received!");
		use_celsius = use_celsius_t->value->int8;

		persist_write_int(KEY_USE_CELSIUS, use_celsius);
	}

	if (temperature_t) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEMPERATURE received!");

  		snprintf(temp_buffer, sizeof(temp_buffer), "Feels like %d", (int)temperature_t->value->int32);
	}

	if (temperature_c_t) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEMPERATURE_IN_C received!");

  		snprintf(temp_buffer, sizeof(temp_c_buffer), "Feels like %d", (int)temperature_t->value->int32);
	}

	if (conditions_t) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CONDITIONS received!");

  		snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_t->value->cstring);
  		text_layer_set_text(s_conditions_layer, conditions_buffer);
  	}

	if (use_celsius == 1) {
		text_layer_set_text(s_temp_layer, temp_c_buffer);
	} else {
		text_layer_set_text(s_temp_layer, temp_buffer);
	}

	update_layers();
}

static void main_window_load(Window *window) {
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CYNTHE_25));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CYNTHE_22));
	s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CYNTHE_14));
	weather_layer = layer_create(GRect(144, 0, 144, 168));

	s_ampm_layer = text_layer_create(GRect(0, 96, 142, 168));
	text_layer_set_background_color(s_ampm_layer, GColorClear);
	text_layer_set_font(s_ampm_layer, s_time_font);
	text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentRight);

	s_date_layer = text_layer_create(GRect(0, 115, 141, 168));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);

	s_time_layer = text_layer_create(GRect(0, 132, 142, 168));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);

	s_temp_layer = text_layer_create(GRect(0, 0, 141, 168));
	text_layer_set_background_color(s_temp_layer, GColorClear);
	text_layer_set_font(s_temp_layer, s_weather_font);
	text_layer_set_text_alignment(s_temp_layer, GTextAlignmentRight);
	text_layer_set_text(s_temp_layer, "Updating");

	s_conditions_layer = text_layer_create(GRect(0, 15, 141, 168));
	text_layer_set_background_color(s_conditions_layer, GColorClear);
	text_layer_set_font(s_conditions_layer, s_weather_font);
	text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentRight);
	text_layer_set_text(s_conditions_layer, "Weather");

	layer_add_child(window_get_root_layer(window), weather_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ampm_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(weather_layer, text_layer_get_layer(s_temp_layer));
	layer_add_child(weather_layer, text_layer_get_layer(s_conditions_layer));

	if (persist_exists(KEY_SHOW_WEATHER)) {
		show_weather = persist_read_int(KEY_SHOW_WEATHER);
	}

	if (persist_exists(KEY_USE_CELSIUS)) {
		use_celsius = persist_read_int(KEY_USE_CELSIUS);
	}

	update_layers();
	update_time();
}

static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_ampm_layer);
	text_layer_destroy(s_date_layer);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
	if (show_weather == 0) {
		// do not animate
	} else {
		animate_layers();
	}
}


static void init() {
	s_main_window = window_create();

	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(s_main_window, true);
	tick_timer_service_subscribe(HOUR_UNIT, tick_handler);
	accel_tap_service_subscribe(tap_handler);

	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}