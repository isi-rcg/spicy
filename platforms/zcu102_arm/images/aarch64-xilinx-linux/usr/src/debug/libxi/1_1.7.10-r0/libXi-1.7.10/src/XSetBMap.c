/************************************************************

Copyright 1989, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

Copyright 1989 by Hewlett-Packard Company, Palo Alto, California.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Hewlett-Packard not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
HEWLETT-PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * XSetDeviceButtonMapping - Set the button mapping of an extension device.
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/extensions/XI.h>
#include <X11/extensions/XIproto.h>
#include <X11/Xlibint.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/extutil.h>
#include "XIint.h"

/* returns either  DeviceMappingSuccess or DeviceMappingBusy  */

int
XSetDeviceButtonMapping(
    register Display	*dpy,
    XDevice		*device,
    unsigned char	 map[],
    int			 nmap)
{
    register xSetDeviceButtonMappingReq *req;
    xSetDeviceButtonMappingReply rep;
    XExtDisplayInfo *info = XInput_find_display(dpy);

    LockDisplay(dpy);
    if (_XiCheckExtInit(dpy, XInput_Initial_Release, info) == -1)
	return (NoSuchExtension);
    GetReq(SetDeviceButtonMapping, req);
    req->reqType = info->codes->major_opcode;
    req->ReqType = X_SetDeviceButtonMapping;
    req->map_length = nmap;
    req->length += (nmap + 3) >> 2;
    req->deviceid = device->device_id;

    Data(dpy, (char *)map, (long)nmap);	/* note that map is char[] */
    if (_XReply(dpy, (xReply *) & rep, 0, xFalse) == 0)	/* suppress error   */
	rep.status = MappingSuccess;
    UnlockDisplay(dpy);
    SyncHandle();
    return ((int)rep.status);
}
