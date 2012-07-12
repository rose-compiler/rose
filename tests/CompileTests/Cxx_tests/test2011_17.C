/*
The following example doesn't compile with ROSE. In the unparsed code, there is no space between & and =, which
creates the &= operator. It seems like a simple bug to fix - there should be a space between the ampersand and the
assignment operator.


fnCallDefaultParameterBug.C
-----------------------------
struct TCP
{
	static TCP& Default();
};

struct TCPSendData
{
	TCPSendData(const TCP& = TCP::Default());
};
-----------------------------


Result of unparsing with ROSE:
-----------------------------
struct TCP 
{
  static TCP &Default();
}
;

struct TCPSendData 
{
// **** Notice the lack of space between & and = here. *****
  TCPSendData(const struct TCP &=TCP:: Default ());
}
;
-----------------------------
*/

struct TCP
   {
     static TCP& Default();
   };

struct TCPSendData
   {
     TCPSendData(const TCP& = TCP::Default());
   };
