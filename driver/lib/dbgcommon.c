#include <ntddk.h>

#include <ntstrsafe.h>

#include "dbgcode.h"
#include "usbip_proto.h"

#ifdef DBG

int
dbg_snprintf(char *buf, int size, const char *fmt, ...)
{
	va_list	arglist;
	size_t	len;
	NTSTATUS	status;

	va_start(arglist, fmt);
	status = RtlStringCchVPrintfA(buf, size, fmt, arglist);
	va_end(arglist);

	if (NT_ERROR(status))
		return 0;
	status = RtlStringCchLengthA(buf, size, &len);
	if (NT_ERROR(status))
		return 0;
	return (int)len;
}

static namecode_t	namecodes_usbip_command[] = {
	K_V(USBIP_CMD_SUBMIT)
	K_V(USBIP_CMD_UNLINK)
	K_V(USBIP_RET_SUBMIT)
	K_V(USBIP_RET_UNLINK)
	{0,0}
};

const char *
dbg_command(UINT32 command)
{
	return dbg_namecode(namecodes_usbip_command, "usbip command", command);
}

const char *
dbg_usbip_hdr(struct usbip_header *hdr)
{
	static char	buf[512];
	int	n;

	n = dbg_snprintf(buf, 512, "cmd:%s,seq:%u,%s,ep:%u", dbg_command(hdr->base.command), hdr->base.seqnum, hdr->base.direction ? "in" : "out", hdr->base.ep);
	switch (hdr->base.command) {
	case USBIP_CMD_SUBMIT:
		dbg_snprintf(buf + n, 512 - n, ",tlen:%d,intv:%d",
			hdr->u.cmd_submit.transfer_buffer_length, hdr->u.cmd_submit.interval);
		break;
	case USBIP_RET_SUBMIT:
		dbg_snprintf(buf + n, 512 - n, ",alen:%u", hdr->u.ret_submit.actual_length);
		break;
	case USBIP_CMD_UNLINK:
		dbg_snprintf(buf + n, 512 - n, ",unlinkseq:%u", hdr->u.cmd_unlink.seqnum);
		break;
	case USBIP_RET_UNLINK:
		dbg_snprintf(buf + n, 512 - n, ",st:%u", hdr->u.ret_unlink.status);
		break;
	default:
		break;
	}
	return buf;
}

#endif