/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2019 PJRC.COM, LLC.
 * Modified by CryHam (2020)  added mouse idle and accel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "usb_dev.h"
#include "usb_mouse.h"
#include "core_pins.h" // for yield()
#include <string.h> // for memcpy()
#include <math.h> // for pow
#include "avr/pgmspace.h" // for PROGMEM, DMAMEM, FASTRUN
#include "debug/printf.h"
#include "core_pins.h"
#include "kc_params.h"

#ifdef MOUSE_INTERFACE // defined by usb_dev.h -> usb_desc.h

// which buttons are currently pressed
uint8_t usb_mouse_buttons_state=0;
uint8_t usb_mouse_buttons_update=0;

//#define DEFAULT_XRES 640
//#define DEFAULT_YRES 480

//#define DEFAULT_XRES 800
//#define DEFAULT_YRES 600

//#define DEFAULT_XRES 1024
//#define DEFAULT_YRES 768

//#define DEFAULT_XRES 1280
//#define DEFAULT_YRES 720

//#define DEFAULT_XRES 1280
//#define DEFAULT_YRES 800

#define DEFAULT_XRES 1366
#define DEFAULT_YRES 768

//#define DEFAULT_XRES 1440
//#define DEFAULT_YRES 900

//#define DEFAULT_XRES 1920
//#define DEFAULT_YRES 1080

//#define DEFAULT_XRES 2560
//#define DEFAULT_YRES 1440

//#define DEFAULT_XRES 2560
//#define DEFAULT_YRES 1600

//#define DEFAULT_XRES 2880
//#define DEFAULT_YRES 1800

//#define DEFAULT_XRES 3840
//#define DEFAULT_YRES 2160

//#define DEFAULT_XRES 7680
//#define DEFAULT_YRES 4320


#define DEFAULT_XSCALE ((0x80000000ul+DEFAULT_XRES/2)/DEFAULT_XRES)
#define DEFAULT_YSCALE ((0x80000000ul+DEFAULT_YRES/2)/DEFAULT_YRES)

static uint16_t usb_mouse_resolution_x=DEFAULT_XRES;
static uint16_t usb_mouse_resolution_y=DEFAULT_YRES;
static uint16_t usb_mouse_position_x=DEFAULT_XRES/2;
static uint16_t usb_mouse_position_y=DEFAULT_YRES/2;
static uint32_t usb_mouse_scale_x=DEFAULT_XSCALE;
static uint32_t usb_mouse_scale_y=DEFAULT_YSCALE;
static uint32_t usb_mouse_offset_x=DEFAULT_XSCALE/2-1;
static uint32_t usb_mouse_offset_y=DEFAULT_YSCALE/2-1;


#define TX_NUM     4
#define TX_BUFSIZE 32
static transfer_t tx_transfer[TX_NUM] __attribute__ ((used, aligned(32)));
DMAMEM static uint8_t txbuffer[TX_NUM * TX_BUFSIZE] __attribute__ ((aligned(32)));
static uint8_t tx_head=0;
#if MOUSE_SIZE > TX_BUFSIZE
#error "Internal error, transmit buffer size is too small for mouse endpoint"
#endif


void usb_mouse_configure(void)
{
	memset(tx_transfer, 0, sizeof(tx_transfer));
	tx_head = 0;
	usb_config_tx(MOUSE_ENDPOINT, MOUSE_SIZE, 0, NULL);
}


// Set the mouse buttons.  To create a "click", 2 calls are needed,
// one to push the button down and the second to release it
int usb_mouse_buttons(uint8_t left, uint8_t middle, uint8_t right, uint8_t back, uint8_t forward)
{
        uint8_t mask=0;

        if (left) mask    |= 1;
        if (middle) mask  |= 4;
        if (right) mask   |= 2;
        if (back) mask    |= 8;
        if (forward) mask |= 16;
        usb_mouse_buttons_state = mask;
        return usb_mouse_move(0, 0, 0, 0);
}


static uint8_t transmit_previous_timeout=0;

// When the PC isn't listening, how long do we wait before discarding data?
#define TX_TIMEOUT_MSEC 30


//  var
//  mouse move speed vars  ---
uint32_t old_ti = 0, time = 0,
	old_ti_mx=0, old_ti_my=0,
	old_ti_wx=0, old_ti_wy=0;
int8_t old_iwx=0, old_iwy=0;

float mx_holdtime = 0.f, my_holdtime = 0.f;
int mx_delay=10000, my_delay=10000;  // for status
int mx_speed=1, my_speed=1;

//  accel params --
const static float
	decay = 1.0 - 0.02, hold_max = 2.0,
	pow_fast = 0.5, pow_slow = 2,
	spd_mul[2] = { 2.0, 1.0 };
const static int spd_max = 48,
	delay_max[2] = { 10000,20000 },
	delay_mul[2] = { 20000,40000 };

//  held mouse keys --
volatile int8_t
	Mouse_input_x = 0, Mouse_input_y = 0,
	Mouse_wheel_x = 0, Mouse_wheel_y = 0, Mouse_shift = 0;
//  wheel dt max, min accel
uint32_t wdtx = 200000, wdty = 200000;
const static uint32_t wmin = 20000;


#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#endif


//  mouse idle update  accel, speed
//..............................................................................
void usb_mouse_idle()
{
	int shift = Mouse_shift;
	//  update
	uint32_t ti = micros(), dt = ti - old_ti;  old_ti = ti;
	int mx_move = Mouse_input_x ? 1 : 0;
	int my_move = Mouse_input_y ? 1 : 0;

	//  mouse send interval  par-
	float htx = min(hold_max, mx_holdtime);
	float hty = min(hold_max, my_holdtime);

	const float s = par.mkSpeed/100.f, a = par.mkAccel/100.f;  // kc

	mx_delay = a * delay_max[shift] - pow(htx, pow_fast) * delay_mul[shift];  mx_delay = max(0, mx_delay);
	my_delay = a * delay_max[shift] - pow(hty, pow_fast) * delay_mul[shift];  my_delay = max(0, my_delay);

	mx_speed = s * pow(htx, pow_slow *a) * spd_mul[shift] + 1;  mx_speed = min(spd_max, mx_speed);
	my_speed = s * pow(hty, pow_slow *a) * spd_mul[shift] + 1;  my_speed = min(spd_max, my_speed);

	//  accel
	if (mx_move && !shift)  mx_holdtime += 0.000001f * dt;
	if (my_move && !shift)  my_holdtime += 0.000001f * dt;

	//  decel  const freq
	if (dt > 60000)  dt = 60000;  // min 16 fps
	const uint32_t iv = 10000;  // interval 100 fps
	time += dt;
	while (time >= iv)
	{	time -= iv;
		if (!mx_move || shift)  mx_holdtime *= decay;
		if (!my_move || shift)  my_holdtime *= decay;
	}

	//  mouse send, int
	int mx_send = mx_speed>1 || (mx_move && ti - old_ti_mx > mx_delay) ? 1 : 0;
	int my_send = my_speed>1 || (my_move && ti - old_ti_my > my_delay) ? 1 : 0;

	int8_t x = mx_send ? Mouse_input_x * (int16_t)(mx_speed) / 8 : 0;
	int8_t y = my_send ? Mouse_input_y * (int16_t)(my_speed) / 8 : 0;


	//  wheel, repeat
	int8_t iwx = Mouse_wheel_x, iwy = Mouse_wheel_y;
	int8_t swx = 0, swy = 0;
	int wmax = (260-par.mkWhSpeed)/100.f * 100000, wdec = min(wmax, par.mkWhAccel/100.f * 10000);  // kc

	int8_t wxon = iwx && !old_iwx;  if (wxon)  wdtx = wmax;
	int8_t wyon = iwy && !old_iwy;  if (wyon)  wdty = wmax;
	if (wxon || (iwx && ti - old_ti_wx > wdtx)){  swx = iwx;  old_ti_wx = ti;  wdtx -= wdec;  if (wdtx < wmin) wdtx = wmin; }
	if (wyon || (iwy && ti - old_ti_wy > wdty)){  swy = iwy;  old_ti_wy = ti;  wdty -= wdec;  if (wdty < wmin) wdty = wmin; }
	old_iwx = iwx;  old_iwy = iwy;


	//  send  ---
	if (x || y || usb_mouse_buttons_update || swx || swy)
		usb_mouse_move(x, y, -swy, swx);


	//  clear status and state
	usb_mouse_buttons_update = 0;

	if (mx_send) old_ti_mx = ti;
	if (my_send) old_ti_my = ti;
}


static int usb_mouse_transmit(const uint8_t *data, uint32_t len)
{
        if (!usb_configuration) return -1;
        uint32_t head = tx_head;
        transfer_t *xfer = tx_transfer + head;
        uint32_t wait_begin_at = systick_millis_count;
        while (1) {
                uint32_t status = usb_transfer_status(xfer);
                if (!(status & 0x80)) {
                        if (status & 0x68) {
                                // TODO: what if status has errors???
                                printf("ERROR status = %x, i=%d, ms=%u\n",
                                        status, tx_head, systick_millis_count);
                        }
                        transmit_previous_timeout = 0;
                        break;
                }
                if (transmit_previous_timeout) return -1;
                if (systick_millis_count - wait_begin_at > TX_TIMEOUT_MSEC) {
                        // waited too long, assume the USB host isn't listening
                        transmit_previous_timeout = 1;
                        return -1;
                }
                if (!usb_configuration) return -1;
                yield();
        }
	delayNanoseconds(30); // TODO: why is status ready too soon?
        uint8_t *buffer = txbuffer + head * TX_BUFSIZE;
        memcpy(buffer, data, len);
        usb_prepare_transfer(xfer, buffer, len, 0);
        arm_dcache_flush_delete(buffer, TX_BUFSIZE);
        usb_transmit(MOUSE_ENDPOINT, xfer);
        if (++head >= TX_NUM) head = 0;
        tx_head = head;
        return 0;
}


// Move the mouse.  x, y and wheel are -127 to 127.  Use 0 for no movement.
int usb_mouse_move(int8_t x, int8_t y, int8_t wheel, int8_t horiz)
{
        //printf("move\n");
        if (x == -128) x = -127;
        if (y == -128) y = -127;
        if (wheel == -128) wheel = -127;
        if (horiz == -128) horiz = -127;

	uint8_t buffer[6];
        buffer[0] = 1;
        buffer[1] = usb_mouse_buttons_state;
        buffer[2] = x;
        buffer[3] = y;
        buffer[4] = wheel;
        buffer[5] = horiz; // horizontal scroll
	return usb_mouse_transmit(buffer, 6);
}

int usb_mouse_position(uint16_t x, uint16_t y)
{
	if (x >= usb_mouse_resolution_x) x = usb_mouse_resolution_x - 1;
	usb_mouse_position_x = x;
	if (y >= usb_mouse_resolution_y) y = usb_mouse_resolution_y - 1;
	usb_mouse_position_y = y;
	uint8_t buffer[5];
	buffer[0] = 2;
	uint32_t val32 = usb_mouse_position_x * usb_mouse_scale_x + usb_mouse_offset_x;
	 //printf("position: %u -> %u", usb_mouse_position_x, val32);
	buffer[1] = val32 >> 16;
	buffer[2] = val32 >> 24;
	val32 = usb_mouse_position_y * usb_mouse_scale_y + usb_mouse_offset_y;
	 //printf(", %u -> %u\n", usb_mouse_position_y, val32);
	buffer[3] = val32 >> 16;
	buffer[4] = val32 >> 24;
	return usb_mouse_transmit(buffer, 5);
}

void usb_mouse_screen_size(uint16_t width, uint16_t height, uint8_t mac)
{
	if (width < 128) width = 128;
	else if (width > 7680) width = 7680;
	if (height < 128) height = 128;
	else if (height > 7680) height = 7680;
	usb_mouse_resolution_x = width;
	usb_mouse_resolution_y = height;
	usb_mouse_position_x = width / 2;
	usb_mouse_position_y = height / 2;
	usb_mouse_scale_x = (0x80000000ul + (width >> 1)) / width;
	usb_mouse_scale_y = (0x80000000ul + (height >> 1)) / height;
	usb_mouse_offset_x = (usb_mouse_scale_x >> 1) - 1;
	usb_mouse_offset_y = (usb_mouse_scale_y >> 1) - 1;
	if (mac) {
		// ugly workaround for Mac's HID coordinate scaling:
		// http://lists.apple.com/archives/usb/2011/Jun/msg00032.html
		usb_mouse_offset_x += 161061273ul;
		usb_mouse_offset_y += 161061273ul;
		usb_mouse_scale_x = (1825361101ul + (width >> 1)) / width;
		usb_mouse_scale_y = (1825361101ul + (height >> 1)) / height;
	}
}


#endif // MOUSE_INTERFACE
