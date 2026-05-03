/*Using LVGL with Arduino requires some extra steps:
 * Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include <TFT_eSPI.h>
#include"GT911.h"
#include"ui.h"


//static const uint16_t screenWidth  = 320;
//static const uint16_t screenHeight = 480;


static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;
GT911 ts = GT911();
GTPoint *oldPoint;


static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *btPower;
lv_obj_t *screenMain;
lv_obj_t *label;
lv_obj_t * labelTempSelected;
lv_obj_t *btnTempBackground;
//ajout logo
LV_IMG_DECLARE(logo2);
LV_IMG_DECLARE(airconditionnerMain);
//fin ajout logo

static lv_style_t style;
  


static void event_handler_btn(lv_obj_t * obj, lv_event_t event){
    if(event.code == LV_EVENT_CLICKED) {
        if (obj == btn1){
        lv_label_set_text(labelTempSelected, "21");
        }
        else if (obj == btn2){
          lv_label_set_text(labelTempSelected, "23");
        }
    }
}


#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/

void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    Serial.println("dans touchpad");
    uint16_t touchX, touchY;

    //bool touched = tft.getTouch( &touchX, &touchY, 600 );

    GTPoint *p = ts.getPoints();
    uint8_t touched = ts.touched(GT911_MODE_POLLING);

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

       
        data->point.x = p->x;
        data->point.y = p->y;

        Serial.print( "Data x " );
        Serial.println( p->x );

        Serial.print( "Data y " );
        Serial.println( p->y );
    }
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    GTPoint *p = ts.getPoints();
    uint8_t touches = ts.touched(GT911_MODE_POLLING);

    if(touches) {
        data->state = LV_INDEV_STATE_PR;

        // Adapter les coordonnées ici (par exemple rotation 90°)
        data->point.x = 480-p->y;
        //data->point.y = 320 - p->x;
        data->point.y =  p->x;

        //Serial.print("X = "); Serial.print(data->point.x);
        //Serial.print("\tY = "); Serial.println(data->point.y);

        oldPoint->x = data->point.x;
        oldPoint->y = data->point.y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    //tft.setRotation( 3 ); /* Landscape orientation, flipped */

    //tft.setRotation( 1 ); /* Landscape orientation, flipped  ok paysage pas pas pour touch 480*320*/
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    ts.begin();
    ts.setRotation(GT911::Rotate::_90);
    oldPoint = ts.getPoints();


  

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

    /*Initialize the display*/
    //static lv_disp_t disp_drv;
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb=my_input_read;
    lv_indev_drv_register( &indev_drv );
    

ui_init();


  


  
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(1);
}