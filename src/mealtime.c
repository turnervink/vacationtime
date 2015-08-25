#include <pebble.h>
#include <localize.h>

#define KEY_SHOW_WEATHER 0
#define KEY_USE_CELSIUS 1
#define KEY_TEMPERATURE 2
#define KEY_TEMPERATURE_IN_C 3
#define KEY_CONDITIONS 4

// check weather update is working every hour

static Window *s_main_window;
static TextLayer *s_time_layer, *s_ampm_layer, *s_date_layer, *s_temp_layer, *s_conditions_layer;
static Layer *weather_layer, *batt_layer;
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

static void init_animation() {
	GRect firststart = GRect(0, 175, 142, 168);
	GRect secondstart = GRect(0, 175, 141, 168);
	GRect thirdstart = GRect(0, 175, 142, 168);

	GRect first = GRect(0, 132, 142, 168);
	GRect second = GRect(0, 115, 141, 168);
	GRect third = GRect(0, 96, 142, 168);

	int animlen = 700;

	animate_layer(text_layer_get_layer(s_ampm_layer), &thirdstart, &third, animlen, 0);
	animate_layer(text_layer_get_layer(s_date_layer), &secondstart, &second, animlen, 0);
	animate_layer(text_layer_get_layer(s_time_layer), &firststart, &first, animlen, 0);
}

static void set_text_colors(GColor color) {
	text_layer_set_text_color(s_ampm_layer, color);
	text_layer_set_text_color(s_time_layer, color);
	text_layer_set_text_color(s_date_layer, color);
	text_layer_set_text_color(s_temp_layer, color);
	text_layer_set_text_color(s_conditions_layer, color);
}

static void batt_layer_draw(Layer *layer, GContext *ctx) {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	int hour = tick_time->tm_hour;

	BatteryChargeState state = battery_state_service_peek();
 	int pct = state.charge_percent;

	if(hour >= 21) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorBlack));
		#else
			graphics_context_set_fill_color(ctx, GColorWhite);
		#endif
	} else if(hour >= 18) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorOxfordBlue));
		#else
			graphics_context_set_fill_color(ctx, GColorWhite);
		#endif
	} else if(hour >= 15) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorSunsetOrange));
		#else
			graphics_context_set_fill_color(ctx, GColorBlack);
		#endif
	} else if(hour >= 12) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorVividCerulean));
		#else
			graphics_context_set_fill_color(ctx, GColorBlack);
		#endif
	} else if(hour >= 9) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorVividCerulean));
		#else
			graphics_context_set_fill_color(ctx, GColorBlack);
		#endif
	} else if(hour >= 6) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorVividCerulean));
		#else
			graphics_context_set_fill_color(ctx, GColorBlack);
		#endif
	} else if(hour >= 3) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorVeryLightBlue));	
		#else
			graphics_context_set_fill_color(ctx, GColorWhite);
		#endif
	} else if(hour >= 0) {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorBlack));
		#else
			graphics_context_set_fill_color(ctx, GColorWhite);
		#endif
	} else {
		#ifdef PBL_COLOR
			graphics_context_set_fill_color(ctx, gcolor_legible_over(GColorRed));
		#else
			graphics_context_set_fill_color(ctx, GColorBlack);
		#endif
	}

	int totlen = 168;
	graphics_fill_rect(ctx, GRect(0, 168, 3, -(totlen-(totlen*(100-pct))/100)), 0, GCornerNone);
}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	int hour = tick_time->tm_hour;

	if(hour >= 21) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, _("night"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(gcolor_legible_over(GColorBlack));
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else if(hour >= 18) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, _("evening"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorOxfordBlue);
			set_text_colors(gcolor_legible_over(GColorOxfordBlue));
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else if(hour >= 15) {
		text_layer_set_text(s_ampm_layer, _("late"));
		text_layer_set_text(s_time_layer, _("afternoon"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorSunsetOrange);
			set_text_colors(gcolor_legible_over(GColorSunsetOrange));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 12) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, _("afternoon"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVividCerulean);
			set_text_colors(gcolor_legible_over(GColorVividCerulean));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 9) {
		text_layer_set_text(s_ampm_layer, _("late"));
		text_layer_set_text(s_time_layer, _("morning"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVividCerulean);
			set_text_colors(gcolor_legible_over(GColorVividCerulean));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 6) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, _("morning"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVividCerulean);
			set_text_colors(gcolor_legible_over(GColorVividCerulean));
		#else
			window_set_background_color(s_main_window, GColorWhite);
			set_text_colors(GColorBlack);
		#endif
	} else if(hour >= 3) {
		text_layer_set_text(s_ampm_layer, _("early"));
		text_layer_set_text(s_time_layer, _("morning"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorVeryLightBlue);
			set_text_colors(gcolor_legible_over(GColorVeryLightBlue));	
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else if(hour >= 0) {
		text_layer_set_text(s_ampm_layer, " ");
		text_layer_set_text(s_time_layer, _("midnight"));
		#ifdef PBL_COLOR
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(gcolor_legible_over(GColorBlack));
		#else
			window_set_background_color(s_main_window, GColorBlack);
			set_text_colors(GColorWhite);
		#endif
	} else {
		text_layer_set_text(s_ampm_layer, _("error"));
		text_layer_set_text(s_time_layer, _("error"));
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
	// text_layer_set_text(s_date_layer, date_buffer);

	// For localization
	if (strcmp(date_buffer, "Monday") == 0) {
		text_layer_set_text(s_date_layer, _("Monday"));
	} else if(strcmp(date_buffer, "Tuesday") == 0) {
		text_layer_set_text(s_date_layer, _("Tuesday"));
	} else if(strcmp(date_buffer, "Wednesday") == 0) {
		text_layer_set_text(s_date_layer, _("Wednesday"));
	} else if(strcmp(date_buffer, "Thursday") == 0) {
		text_layer_set_text(s_date_layer, _("Thursday"));
	} else if(strcmp(date_buffer, "Friday") == 0) {
		text_layer_set_text(s_date_layer, _("Friday"));
	} else if(strcmp(date_buffer, "Saturday") == 0) {
		text_layer_set_text(s_date_layer, _("Saturday"));
	} else if(strcmp(date_buffer, "Sunday") == 0) {
		text_layer_set_text(s_date_layer, _("Sunday"));
	} else {
		text_layer_set_text(s_date_layer, date_buffer);
	}
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

  		snprintf(temp_c_buffer, sizeof(temp_c_buffer), "Feels like %d", (int)temperature_c_t->value->int32);
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

	s_ampm_layer = text_layer_create(GRect(0, 175, 144, 168));
	text_layer_set_background_color(s_ampm_layer, GColorClear);
	text_layer_set_font(s_ampm_layer, s_time_font);
	text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentRight);

	s_date_layer = text_layer_create(GRect(0, 175, 144, 168));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);

	s_time_layer = text_layer_create(GRect(0, 175, 144, 168));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);

	s_temp_layer = text_layer_create(GRect(0, 0, 141, 168));
	text_layer_set_background_color(s_temp_layer, GColorClear);
	text_layer_set_font(s_temp_layer, s_weather_font);
	text_layer_set_text_alignment(s_temp_layer, GTextAlignmentRight);

	s_conditions_layer = text_layer_create(GRect(0, 15, 141, 168));
	text_layer_set_background_color(s_conditions_layer, GColorClear);
	text_layer_set_font(s_conditions_layer, s_weather_font);
	text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentRight);

	weather_layer = layer_create(GRect(144, 0, 144, 168));

	batt_layer = layer_create(GRect(-5, 0, 144, 168));
	layer_set_update_proc(batt_layer, batt_layer_draw);

	layer_add_child(window_get_root_layer(window), weather_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ampm_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), batt_layer);
	layer_add_child(weather_layer, text_layer_get_layer(s_temp_layer));
	layer_add_child(weather_layer, text_layer_get_layer(s_conditions_layer));

	if (persist_exists(KEY_SHOW_WEATHER)) {
		show_weather = persist_read_int(KEY_SHOW_WEATHER);
	}

	if (persist_exists(KEY_USE_CELSIUS)) {
		use_celsius = persist_read_int(KEY_USE_CELSIUS);
	}

	update_layers(); // show/hide weather
	update_time();
}

static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_ampm_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_temp_layer);
	text_layer_destroy(s_conditions_layer);
	layer_destroy(batt_layer);
	layer_destroy(weather_layer);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
	fonts_unload_custom_font(s_weather_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();

	// Update weather every hour
	if(tick_time->tm_min == 0) {
		// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

		// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

		// Send the message!
		app_message_outbox_send();
	}
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
	if (show_weather == 0) {
		// do not animate weather_layer
	} else {
		animate_layers(); // animate weather_layer
	}

	// animate battery bar
	GRect startbatt = GRect(-5,0,144,168);
	GRect finishbatt = GRect(0,0,144,168);

	animate_layer(batt_layer, &startbatt, &finishbatt, 1000, 0);
	animate_layer(batt_layer, &finishbatt, &startbatt, 1000, 5000);
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

	init_animation(); // animate text layers on launch
}

static void deinit() {
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
	init();
	locale_init();
	app_event_loop();
	deinit();
}