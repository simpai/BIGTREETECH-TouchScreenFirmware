#include "Settings.h"
#include "includes.h"


SETTINGS infoSettings;


void menuInfo(void)
{
  const char* hardware = "Board   : BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = "Firmware: "HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;
  
  u16 HW_X = (LCD_WIDTH - my_strlen((u8 *)hardware)*BYTE_WIDTH)/2;
  u16 FW_X = (LCD_WIDTH - my_strlen((u8 *)firmware)*BYTE_WIDTH)/2;
  u16 centerY = LCD_HEIGHT/2;
  u16 startX = MIN(HW_X, FW_X);
  
  GUI_Clear(BLACK);

  GUI_DispString(startX, centerY - BYTE_HEIGHT, (u8 *)hardware, 0);
  GUI_DispString(startX, centerY, (u8 *)firmware, 0);

  while(!isPress()) loopProcess();
  while(isPress())  loopProcess();

  infoMenu.cur--;
}

void menuDisconnect(void)
{
  GUI_Clear(BLACK);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO), 0);

  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x00000440;// PA9/PA10

  while(!isPress());
  while(isPress());
  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000008B0;
  infoMenu.cur--;
}


MENUITEMS settingsItems = {
//   title
LABEL_SETTINGS,
// icon                       label
 {{ICON_POWER_OFF,            LABEL_POWER_OFF},
  {ICON_LANGUAGE,             LABEL_LANGUAGE},
  {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};


#define ITEM_BAUDRATE_NUM 2
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
//   icon                       label
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_250000},
};
const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200,250000};
static u8  item_baudrate_i = 0;


void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }	
  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:    mustStoreCmd("M81\n");                                break;
      
      case KEY_ICON_1: 
        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_2:    
        TSC_Calibration();
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;
      
      case KEY_ICON_5:
        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;                
        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.baudrate = item_baudrate[item_baudrate_i];
        Serial_Config(infoSettings.baudrate);
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    loopProcess();		
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
