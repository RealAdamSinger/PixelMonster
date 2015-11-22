#include <pebble.h>

#define textColor PBL_IF_COLOR_ELSE(GColorGreen, GColorWhite)

static Window *window;
static int xPos = 50, yPos = 50, xWait = 0,  yWait = 0, xDif = 0, yDif = 0, mSize=60, row=0, rG=60;
static TextLayer *txtTroph, *txtAlignment, *txtWorldAlignment, *txtLevel, *txtXp, *txtRank, *txtName;
static GFont fontMain;
static Layer *windowLayer, *health, *armour, *background, *trophies, *alignment, *worldAlignment, *level, *xp, *rank, *name, *monsterBox, *monsterLayer;
static PropertyAnimation *s_property_animation;
static GColor monster[60][60];
static Layer *monsterRow[60];
typedef struct {
  GColor fillColor;
  GColor emptyColor;  
  int percent;
} BarContext;

typedef struct {  
  int row;
} RowContext;

static void bt_handler(bool connected) {
  if (connected) {

  } else {     

  }
}
static double randf(double low,double high){
    return (rand()/(double)(RAND_MAX))*abs((low-1)-(high+1))+(low-1);
}

////////////////////////////////////////////////////////////////// DRAWING
static void _drawMonster(Layer *layer, GContext *ctx) {      
    for(int x=0; x < mSize; x++){   
      for(int y=0; y < 6; y++){   
        graphics_context_set_stroke_color(ctx,  monster [x][row]);
        graphics_draw_pixel(ctx, GPoint(x,0+y));              
      }    
    }    
}
static void drawMonster(Layer *layer) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING DRAWTEXTREGION()");
  layer_set_update_proc(layer, _drawMonster);  
  layer_add_child(monsterBox, layer);  
}
static void _drawMonsterBox(Layer *layer, GContext *ctx) {    
    GRect bounds = layer_get_bounds(layer);        
    graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorWhite, GColorWhite));
    graphics_fill_rect(ctx, bounds, 2, GCornerNone); 
}
static void drawMonsterBox(Layer *layer) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING DRAWTEXTREGION()");
  layer_set_update_proc(layer, _drawMonsterBox);  
  layer_add_child(windowLayer, layer);  
}
static void _drawTextRegion(Layer *layer, GContext *ctx) {    
    GRect bounds = layer_get_bounds(layer);        
    graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorMintGreen, GColorBlack));
    graphics_fill_rect(ctx, bounds, 0, GCornerNone); 
}
static void drawTextRegion(Layer *layer) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING DRAWTEXTREGION()");
  layer_set_update_proc(layer, _drawTextRegion);  
  layer_add_child(windowLayer, layer);  
}
static void _drawBar(Layer *layer, GContext *ctx) {  
    // Retrieve this layer's data
    BarContext *anim_ctx = (BarContext*)layer_get_data(layer);
    GRect bounds = layer_get_bounds(layer);        
  int total = bounds.size.w * (anim_ctx->percent)/100;
    graphics_context_set_fill_color(ctx, anim_ctx->fillColor);
    graphics_fill_rect(ctx, GRect(0, 0, total-1, bounds.size.h), 0, GCornerNone); 
    graphics_context_set_fill_color(ctx, anim_ctx->emptyColor);
    graphics_fill_rect(ctx, GRect(total, 0, bounds.size.w, bounds.size.h), 0, GCornerNone); 
}
static void drawBar(Layer *layer, int percent, GColor fillColor, GColor emptyColor){  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING DRAWBAR()");    
   // Store initial values
  BarContext *layerContext = (BarContext*)layer_get_data(layer);
  layerContext->fillColor = fillColor;
  layerContext->emptyColor = emptyColor;  
  layerContext->percent = percent;  
  layer_set_update_proc(layer, _drawBar);    
  layer_add_child(windowLayer, layer);  
}
//////////////////////////////////////////////////////////////////DRAWING END
static void moveMonster(void *data){   
  int r = --rG;
  GRect to_frame;
  GRect from_frame;     
  if(yDif > 0 && r+1 < 60){          
      from_frame = layer_get_frame(monsterRow[r+1]); 
      to_frame = GRect(xPos, yPos+r+1, 60, 1);
      // Create the animation
      s_property_animation = property_animation_create_layer_frame(monsterRow[r], &from_frame, &to_frame);
      // Schedule to occur ASAP with default settings
      animation_schedule((Animation*) s_property_animation);          
    to_frame = GRect(xPos, yPos+r, 60, yDif+1);
  }  else{
    to_frame = GRect(xPos, yPos+r, 60, 1);
  }
    
    from_frame = layer_get_frame(monsterRow[r]); 
    
      // Create the animation
      s_property_animation = property_animation_create_layer_frame(monsterRow[r], &from_frame, &to_frame);
      // Schedule to occur ASAP with default settings
      animation_schedule((Animation*) s_property_animation);  
  if(rG==0)rG=mSize;
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if(xWait < 0){
    xDif = randf(-5,5);
    xWait = randf(2,8);
  }
  --xWait;
  if(yWait < 0){
    yDif = randf(-5,5);
    yWait = randf(2,8);
  }
  --yWait;
  xPos += xDif;  
  yPos += yDif;  
  if((yPos + mSize) > 114) yPos = 54;
  if(yPos < 0) yPos = 0;
  if((xPos + mSize) > 144) xPos = 84;
  if(xPos < 0) xPos = 0;
  // Set start and end
  for(int r = mSize-1; r >= 0; --r){   
//     RowContext *anim_ctx = (RowContext*)layer_get_data(monsterRow[r]);    
    app_timer_register(10*r, moveMonster, NULL);
  }  
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
static void windowLoad(Window *window) {  
  for(int x=0; x < mSize; x++){
    for(int y=0; y < mSize; y++){      
      monster[x][y] = PBL_IF_COLOR_ELSE(GColorFromRGB(y*x, y*3, x*3), GColorWhite);      
    }
  }    
  
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING WINDOWLOAD()");
  // Get the root layer
  windowLayer = window_get_root_layer(window);
  
   // Create GFont  
  fontMain = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MAIN_11));  
  
  //////////////////////////////////////////////////////////////////// Create Titles   
  APP_LOG(APP_LOG_LEVEL_DEBUG, "creating title layout");
  name = layer_create(GRect(0, 0, 144, 10));
  level = layer_create(GRect(0, 11, 50, 10));
  xp = layer_create(GRect(51, 11, 144, 10));
  trophies = layer_create(GRect(0, 22, 50, 10));
  rank = layer_create(GRect(51, 22, 144, 10));
  alignment = layer_create(GRect(0, 33, 50, 10));
  worldAlignment = layer_create(GRect(51, 33, 144, 10));
  drawTextRegion(name);    
  drawTextRegion(level);    
  drawTextRegion(xp);    
  drawTextRegion(trophies);
  drawTextRegion(rank);    
  drawTextRegion(alignment);    
  drawTextRegion(worldAlignment);
  txtName = text_layer_create(layer_get_bounds(name));
  text_layer_set_font(txtName, fontMain); 
  text_layer_set_text_alignment(txtName, GTextAlignmentCenter);
  text_layer_set_background_color(txtName, GColorClear);
  text_layer_set_text_color(txtName, textColor);   
  
  
  
  //////////////////////////////////////////////////////////////////// Create Monster Container
  monsterBox = layer_create(GRect(0, 44, 144, 114));  
  drawMonsterBox(monsterBox);  
  //////////////////////////////////////////////////////////////////// Create Layers for Health and Armour    
  armour = layer_create_with_data(GRect(0, 159, 144, 4), sizeof(BarContext));   
  health = layer_create_with_data(GRect(0 , 164, 144, 4), sizeof(BarContext));   
  drawBar(armour, 80, PBL_IF_COLOR_ELSE(GColorBlue, GColorWhite), PBL_IF_COLOR_ELSE(GColorCeleste, GColorWhite));
  drawBar(health, 40, PBL_IF_COLOR_ELSE(GColorDarkCandyAppleRed, GColorWhite), PBL_IF_COLOR_ELSE(GColorMelon, GColorWhite));  
  
  bt_handler(connection_service_peek_pebble_app_connection());  
}

static void windowUnload(Window *window) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING WINDOWUNLOAD()");
  // Destroy TextLayer  
  connection_service_unsubscribe();  
  layer_destroy(background);
  layer_destroy(armour);
  layer_destroy(health);
  fonts_unload_custom_font(fontMain);
}

static void init() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING INIT()");
  // Create main Window element and assign to pointer
  window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = windowLoad,
    .unload = windowUnload
  });
  window_set_background_color(window, GColorBlack);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bt_handler
  });  
  for(int r=0; r < mSize; r++){
    row = r;
    monsterRow[row] = layer_create(GRect(xPos, yPos+row, 60, 1));   
    RowContext *layerContext = (RowContext*)layer_get_data(monsterRow[row]);
    layerContext->row = row;
    drawMonster(monsterRow[row]);
  }  
  // Register with TickTimerService
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
}

static void deinit() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING DEINIT()");
  // Destroy Window
  window_destroy(window);
}

int main(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RUNNING MAIN()");
  init();
  app_event_loop();
  deinit();
}
