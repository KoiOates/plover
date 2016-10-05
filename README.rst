This file is just instructions for doing some fun stuff with this
fork of Plover in particular.
`Click here to view the main Plover README <README_original.rst>`_

Ploverducken: Prebaked Steno Translation on a C.H.I.P.
=====================================================
**(now with a side of Pi ... gluten free and paleo friendly!)**

This is basically an experimental Plover, modified to reduce
dependencies and to leave out the GUI. The oslayer section
is overridden with a custom keyboard module in
plover/oslayer/nonkeyboard.py to take lines of raw piped keyboard
input on /tmp/plovin, print some debugging info, and send the
finished keystrokes in an hid_gadget_test friendly way via
/tmp/plovout.

This is still very experimental, but here's how to play with it
if anyone else (including my future self) wants to use it as an
example of how to use the power of Plover without having to
have run the software on all the computers you need to use.

The following steps were almost the same getting this to work on
a Raspberry Pi Zero, with differences in bold type. Mostly just
had to change the last line of the starthid script.
Tested this with a UART to USB cable to log in to the Pi and the
although I'm not sure if there's

Step 0: Download, checkout.
---------------------------

In a terminal type:

git clone https://github.com/KoiOates/plover.git

cd plover

git checkout -b ploverducken origin/ploverducken

(I just started using github for my own stuff recently, and only
just now discovered this was the way to mess with branches
when trying to clone this to test everything uploaded correctly!)

Step 1: Compile kernel with necessary modules.
----------------------------------------------

This may not be necessary on all Linux running gadgets,
and definitely **is not necessary on Raspberry Pi Zero, latest Rasbian version**,
but it was a necessary
step before my PocketCHIP could be set up to send keystrokes.

Here is an excellent step by step procedure for cross compiling
the kernel:
www.raspibo.org/wiki/index.php/Compile_the_Linux_kernel_for_Chip:_my_personal_HOWTO
And another excellent guide for compiling on the C.H.I.P. itself:
www.raspibo.org/wiki/index.php/HOW-TO_compile_Chip's_Linux_kernel_and_modules_on_Chip_itself

For the configuration file, I copied in the one from /boot in the
standard image that came with the PocketCHIP and made sure it hadthe usb_f_hid and configfs modules during the "make menuconfig"
step. A copy of the final configuration file I used can be found
in the hidout directory of this project.

Note that although renzo does provide a precompiled kernel for
download in these tutorials, I was unable to test it and still
have the display on my PocketCHIP turn on, and I don't know
enough about these things to know why that is. The display still
worked when I only made the above changes to the default
configuration.

Step 2: Add hid support on boot.
----------------------------------------------------


cd into the hidout directory and enter:

gcc hid_gadget_test.c -o hid_gadget_test


Repeat this step if you edit hid_gadget_test.c to include more
keycodes that can be sent by plover. It has the whole list of
keys that can be entered with {#...} dictionary entries, but only
the ones I actually looked up the hid codes for are not commented
out. Adding mappings for special keys is a work in progress.

There's probably other ways to make this next part work, but this
was the most convenient and stable for me. Copy and paste these
lines to your /etc/rc.local file before the "exit 0" line, using
sudo vim or sudo your-editor-of-choice, changing the user variable
if necessary and plover_dir to reflect the path where you have
this repository cloned:


user="chip" # or user="pi" as the case may be

plover_dir="/home/chip/plover/" #or /home/pi/plover


$plover_dir/hidout/starthid

sudo -u $user mkfifo /tmp/plovin

sudo -u $user mkfifo /tmp/plovout

tail -f /tmp/plovout | $plover_dir/hidout/hid_gadget_test /dev/hidg0 keyboard >> $plover_dir/hidout/hid_gadget.log 2>&1 &


The starthid script is from:
www.raspibo.org/wiki/index.php/Chip:_USB_hid_on_libcomposite
And the hid_gadget_test program is from code found in this kernel
documentation, modified to work more easily with Plover:
www.kernel.org/doc/Documentation/usb/gadget_hid.txt

In my version of PocketCHIP, the g_serial module is loaded by
default, but this conflicts with the usb_f_hid module. Make sure
you're set up where you don't need to serial into the C.H.I.P.
via micro USB port, and then edit /etc/modules to get rid of or
comment out the "g_serial" line, and add "usb_f_hid" in its place. Restart the C.H.I.P.

On **Raspberry Pi**, I believe a "dwc2" line is also necessary in /etc/modules.

Step 3: Run Plover and test the input and output.
-------------------------------------------------

This version of Plover should just run without having to install
much, if any, dependencies, except I had issues with setuptools
before it would work on C.H.I.P. I couldn't get it to work with
just "pip install". I had to manually download the .whl file
from https://pypi.python.org/pypi/setuptools and instead run
"pip install [name of the wheel file]". Then it's time to plug
in.

Put the micro end of a USB cable in your C.H.I.P. and the other
end into any computer. In one terminal window on the C.H.I.P.,
run ./launch in the Plover root directory. Then in another run
./hidout/stdin_fifo_dummy.py to test the setup. All this program
does is take lines of keyboard input and send them down the
/tmp/plovin pipe for consumption by Plover. It's kind of like
arpeggiate mode with an enter key to send instead of space, more
for testing than anything, but I have a mode in the Stenoboard
firmware for adding a 'return' after each NKRO keyboard stroke
as a quick fix to try this out using normal steno chording.
https://github.com/KoiOates/StenoFW/tree/nowblinkier

Note that when Plover first starts, it will take a little while
for all of its pieces to warm up on one of these boards. At first
it will send nothing, then it will send stroke definitions, but
it will send translations once everything is initialized.

If anyone is in a position to use this for more than just a proof
of concept on how to set up a Plover-in-the-middle gadget, I'll
update this version of Plover to take advantage of more features
of the main version. As of now, it's just a quick hack to test
things out.

I needed to put this info up anyway in case I end up bricking my
PocketCHIP in my mad quest to turn it into a brain for an old
Stenograph writer, but I hope this will save other people
some trouble figuring out how to do simliar projects.

Here's a YouTube video of me doing absurd things with both of these gadgets:

https://www.youtube.com/watch?v=cNcA81tYH-M
