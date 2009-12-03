LinkReal::LinkReal()
: Link(), notify(nil), linkMonitor(nil)
{
	ConstructorHelper(Link::DefaultRate(), Link::DefaultDelay());
}
