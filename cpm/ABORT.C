static char	mess[] = "Fatal error - program aborted\r\n$";

abort()
{
	bdos(9, mess);
	exit(-1);
}
