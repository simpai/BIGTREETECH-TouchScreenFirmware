#include "fan.h"
#include "includes.h"


//1个title(标题), ITEM_PER_PAGE个item(图标+标签) 
const MENUITEMS fanItems = {
//   title
LABEL_FAN,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_FAN ,                 LABEL_FAN},
  {ICON_FAN_FULL_SPEED,       LABEL_FAN_FULL_SPEED},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_BACK,                 LABEL_BACK},}
};

const char* fanID[] = FAN_ID;
const char* fanCmd[] = FAN_CMD;
const u8    fanMaxPWM[] = FAN_MAX_PWM;

static u8   fanSpeed[FAN_NUM] = {0};
static u8   curIndex = 0;
static bool send_waiting[FAN_NUM] = {false};

void fanSetSpeed(u8 i, u8 speed)
{
  fanSpeed[i] = speed;
}

u8 fanGetSpeed(u8 i)
{
  return fanSpeed[i];
}

void fanSetCurIndex(u8 i)
{
  if(i >= FAN_NUM) return;
  curIndex = i;
}

u8 fanGetCurIndex(u8 i)
{
  return curIndex;
}

/* 设置是否已经发送风扇 */
void fanSetSendWaiting(u8 i, bool isWaiting)
{
  send_waiting[i] = isWaiting;
}

void showFanSpeed(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y-BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)fanID[curIndex], 1);
  GUI_DispDec(CENTER_X-BYTE_WIDTH, CENTER_Y, fanSpeed[curIndex], 3, 1, LEFT);
}

void fanSpeedReDraw(void)
{
  GUI_DispDec(CENTER_X-BYTE_WIDTH, CENTER_Y, fanSpeed[curIndex], 3, 1, LEFT);
}

void menuFan(void)
{
  u8 nowIndex = curIndex;
  u8 nowFanSpeed[FAN_NUM];
  memcpy(nowFanSpeed, fanSpeed, sizeof(fanSpeed));
  KEY_VALUES key_num = KEY_IDLE;	

  menuDrawPage(&fanItems);
  showFanSpeed();
  while(infoMenu.menu[infoMenu.cur] == menuFan)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if (fanSpeed[curIndex] > 0)
          fanSpeed[curIndex]--;
        break;
        
      case KEY_ICON_3:
        if (fanSpeed[curIndex] < fanMaxPWM[curIndex])
          fanSpeed[curIndex]++;
        break;
        
      case KEY_ICON_4:
        curIndex = (curIndex + 1) % FAN_NUM;
        showFanSpeed();
        break;
      
      case KEY_ICON_5:
        fanSpeed[curIndex] = fanMaxPWM[curIndex];
        break;
      
      case KEY_ICON_6:
        fanSpeed[curIndex] = 0;
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    
    if(nowIndex != curIndex)
    {
      nowIndex = curIndex;
      showFanSpeed();
    }
    if(nowFanSpeed[curIndex] != fanSpeed[curIndex])
    {
      nowFanSpeed[curIndex] = fanSpeed[curIndex];
      fanSpeedReDraw();
      if(send_waiting[curIndex] != true)
      {
        send_waiting[curIndex] = true;
        storeCmd("%s ", fanCmd[curIndex]);
      }
    }
    loopProcess();
  }
}
