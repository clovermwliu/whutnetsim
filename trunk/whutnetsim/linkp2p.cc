Linkp2p& Linkp2p::Default()
{ // Get the default link
	if (!defaultp2p) defaultp2p = new Linkp2p(); // Allocate it
	return *defaultp2p;                           // Return it
}

Linkp2p::Linkp2p()
: LinkReal(), pPeer(nil), linkAnimation(nil), usedAnim(0)
{
	count++;
}
