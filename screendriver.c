/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <ED060SC4.h>
#include "net/netstack.h"
#include "dev/button-sensor.h"



/*---------------------------------------------------------------------------*/
PROCESS(screendriver_process, "screendriver process");

AUTOSTART_PROCESSES(&screendriver_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(screendriver_process, ev, data)
{

	static but_counter =0;
	PROCESS_BEGIN();

	init_board();
  power_on();

  subclear(0xAA);
  subclear(0xAA);
  subclear(0xAA);
  subclear(0xAA);

	char line1[] = "IETF 90";

	char line3[] = "6LoWPAN CoAP";
	char line4[] = "Alex Roscoe";
	char line5[] = "Drew Taylor";

	set_line(&line1,300, 200);

	set_line(&line3,175, 300);
	set_line(&line4,180, 350);
	set_line(&line5,180, 400);

     power_off();

  printf("*** Done\n");
	PROCESS_END();
}
 /*---------------------------------------------------------------------------*/
