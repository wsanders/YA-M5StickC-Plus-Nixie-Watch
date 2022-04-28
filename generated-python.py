// https://github.com/wsanders/YA-M5StickC-Plus-Nixie-Watch by wsanders

from m5stack import *
from m5ui import *
from uiflow import *
import nvs
lcd.setRotation(1)
import wifiCfg
import time
import ntptime
from easyIO import *

setScreenColor(0x000000)


Brightness = None
TZoffset = None
XtempstrX = None
batpct = None
hr = None
hr1 = None
mnt = None
hr2 = None
mnt1 = None
mnt2 = None

// Move this line down to the multiBtnCb_AB function
// wifiCfg.doConnect('MYSSID', 'MYPASS')

image0 = M5Img(42, 5, "res/error.jpg", False)
image1 = M5Img(42, 65, "res/error.jpg", False)
image2 = M5Img(42, 125, "res/error.jpg", False)
image3 = M5Img(42, 185, "res/error.jpg", False)

from numbers import Number


# Describe this function...
def adjBrightness():
  global Brightness, TZoffset, XtempstrX, batpct, hr, hr1, mnt, hr2, mnt1, mnt2
  Brightness = (Brightness if isinstance(Brightness, Number) else 0) + 20
  if Brightness > 100:
    Brightness = 0
  nvs.write(str('Brightness'), Brightness)


def multiBtnCb_AB():
  global Brightness, TZoffset, XtempstrX, batpct, hr, hr1, mnt, hr2, mnt1, mnt2
  axp.setLcdBrightness(Brightness)
  lcd.rect(0, 0, 220, 30, fillcolor=0x000000)
  lcd.print('CONNECT', 5, 5, 0xff6600)
  // Put wifiCfg.doConnect call here
  wifiCfg.doConnect('MYSSID', 'MYPASS')
  wait(2)
  lcd.rect(0, 0, 220, 30, fillcolor=0x000000)
  lcd.print('SYNC', 5, 5, 0xff6600)
  ntp = ntptime.client(host='us.pool.ntp.org', timezone=TZoffset)
  wait(2)
  lcd.rect(0, 0, 220, 30, fillcolor=0x000000)
  lcd.print('SET RTC', 5, 5, 0xff6600)
  rtc.setTime((ntp.year()), (ntp.month()), (ntp.day()), (ntp.hour()), (ntp.minute()), (ntp.second()))
  lcd.rect(0, 0, 220, 30, fillcolor=0x000000)
  lcd.print(((str('DONE. TZ: ') + str(TZoffset))), 5, 5, 0xff6600)
  wait(2)
  lcd.clear()
  pass
btn.multiBtnCb(btnA,btnB,multiBtnCb_AB)

def buttonA_wasPressed():
  global Brightness, TZoffset, XtempstrX, batpct, hr, hr1, mnt, hr2, mnt1, mnt2
  adjBrightness()
  axp.setLcdBrightness(Brightness)
  pass
btnA.wasPressed(buttonA_wasPressed)

def buttonB_wasPressed():
  global Brightness, TZoffset, XtempstrX, batpct, hr, hr1, mnt, hr2, mnt1, mnt2
  // PST8PDT. Adjust for your timezone.
  if TZoffset == -7:
    TZoffset = -8
  else:
    TZoffset = -7
  nvs.write(str('TZoffset'), TZoffset)
  lcd.rect(0, 0, 220, 30, fillcolor=0x000000)
  lcd.print(((str('TZ: ') + str(TZoffset))), 5, 5, 0xff6600)
  lcd.print('PLS RESYNC', 90, 5, 0xffffff)
  pass
btnB.wasPressed(buttonB_wasPressed)

# YA M5StickC Plus Nixie Watch by wsanders.
#

lcd.rect(0, 0, 220, 30, fillcolor=0x000000)
lcd.print('SETUP', 5, 5, 0xff6600)
XtempstrX = str((nvs.read_int('Brightness')))
try :
  Brightness = int(XtempstrX)
  pass
except:
  Brightness = 40
XtempstrX = str((nvs.read_int('TZoffset')))
try :
  TZoffset = int(XtempstrX)
  pass
except:
  TZoffset = -8
lcd.font(lcd.FONT_Default)
lcd.clear()
axp.setLcdBrightness(Brightness)
image0.setPosition(0, 36)
image1.setPosition(60, 36)
image2.setPosition(120, 36)
image3.setPosition(180, 36)
while True:
  batpct = map_value((axp.getBatVoltage()), 3.7, 4.1, 0, 100)
  lcd.rect(2, 2, 30, 15, color=0xff6600)
  if batpct > 20:
    lcd.rect(6, 5, 4, 9, fillcolor=0x33cc00)
    lcd.rect(6, 5, 4, 9, color=0x33cc00)
  if batpct > 40:
    lcd.rect(12, 5, 4, 9, fillcolor=0x33cc00)
    lcd.rect(12, 5, 4, 9, color=0x33cc00)
  if batpct > 60:
    lcd.rect(18, 5, 4, 9, fillcolor=0x33cc00)
    lcd.rect(18, 5, 4, 9, color=0x33cc00)
  if batpct > 80:
    lcd.rect(24, 5, 4, 9, fillcolor=0x33cc00)
    lcd.rect(24, 5, 4, 9, color=0x33cc00)
  hr = rtc.now()[3]
  if hr <= 9:
    image0.changeImg("res/0.jpg")
    hr2 = str(hr)[0]
    image1.changeImg("res/"+str(hr2)+".jpg")
  else:
    hr1 = str(hr)[0]
    image0.changeImg("res/"+str(hr1)+".jpg")
    hr2 = str(hr)[-1]
    image1.changeImg("res/"+str(hr2)+".jpg")
  mnt = rtc.now()[4]
  if mnt <= 9:
    image2.changeImg("res/0.jpg")
    mnt2 = str(mnt)[0]
    image3.changeImg("res/"+str(mnt2)+".jpg")
  else:
    mnt1 = str(mnt)[0]
    image2.changeImg("res/"+str(mnt1)+".jpg")
    mnt2 = str(mnt)[-1]
    image3.changeImg("res/"+str(mnt2)+".jpg")
  wait(15)
  if (axp.getBatCurrent()) < 0:
    axp.powerOff()
  wait_ms(2)

