# Deadline
The deadline is 3/2. We have only 1.5 days to complete this project.

# Resources
I will suggest that we first reading these materials:

HTTP protocol :RFC7230 (***The format and syntax of whole_message and whole_message is useful for designing parsing function***)
HTTP protocol :RFC7231 (***section 4,5,6,7 will be the most useful part***)

HTTP cache proxy: RFC7234

[Mozilla documentation about cache](https://developer.mozilla.org/en-US/docs/Web/HTTP/Caching)

[Mozilla documentation about http](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview)



[docker reference](https://docs.docker.com/engine/reference/builder/) ***This can be done later(It will be placed in the last place of the priority queue)***

[docker compose](https://docs.docker.com/compose/compose-file/compose-file-v2/) ***This can be done later(It will be placed in the last place of the priority queue)***

I've already attached the related RFC in our git repository.

# COMPLETED
* Setup git repo.
* Wrote TCP/IP connection API
* Wrote function to receive HTTP whole_message stream (*)
* Wrote function to receive HTTP/1.0 whole_message stream (#)

# TODO
* To encapsulate and abstract (*) and (#)
* To write methods for splitting information in whole_message stream (Method/URI/Http Protocol/Token)
* To write methods for receiving HTTP/1.1 whole_message stream
* To write methods for extracting information in whole_message stream(Header field/Message Body)
* To write methods for receive whole_message->send whole_message to server -> receive whole_message -> send whole_message to client

***MILESTONE: working proxy without cache***  
* To write methods for caching whole_message
* To write methods for enabling our cache to tell if a whole_message is expired
* To write methods for enabling our cache to ask the origin server whether a whole_message is valid

***MILESTONE: working cache proxy with single thread***

* To make our proxy multithreaded
 
***MILESTONE: working multithreaded cache proxy***

* To make our cache proxy a daemon

***MILESTONE: working multithreaded cache proxy daemon***

* To make our cache proxy robust (...)

***CURRENT ERRORS***
* When we navigate to espn.com, our program throws a Bad Header exception and exits
* Our program currently blocks if we try to send a POST request


# NOTE
In general, GET/HEAD method is cacheable. While POST method can be cacheable, we're not gonna do this right now. All the other methods will be treated as uncacheable.
One problem is that the whole_message of HEAD method is actually a subset of that of GET method.

For example, if I have a whole_message `GET /docs HTTP/1.0\r\nHost:example.com\r\n\r\n` This is a GET method and let's say we cached it. A subsequent whole_message coming in is
`HEAD /docs HTTP/1.0\r\nHost:example.com\r\n\r\n` This obviously won't be hashed into the same value, but still the whole_message should be the header field of the
previous whole_message, which is already in the cached whole_message.

***DEFINE CACHEABLE***

Cited from RFC2731, if we say a Method is uncacheable, taking ***PUT*** method as example, we mean that if a successful ***PUT*** whole_message passes through a cache that has one or more stored responses for the effective whole_message ***URI***, those stored whole_message will be invalidated.

***CONTROL***
* Expect `Expect = "100-continue"`
    * A server that receives an Expect field-value other than 100-continue MAY respond with a 417 (Expectation Failed) status code toindicate that the unexpected expectation cannot be met.
    * A 100-continue expectation informs recipients that the client is
about to send a (presumably large) message body in this whole_message and
wishes to receive a 100 (Continue) interim whole_message if the
whole_message-line and header fields are not sufficient to cause an
immediate success, redirect, or error whole_message. This allows the
client to wait for an indication that it is worthwhile to send the
message body before actually doing so, which can improve efficiency
when the message body is huge or when the client anticipates that an
error is likely (e.g., when sending a state-changing method, for the
first time, without previously verified authentication credentials).
    * A proxy MUST, upon receiving an HTTP/1.1 (or later) whole_message-line and
a complete header section that contains a 100-continue expectation
and indicates a whole_message message body will follow, either send an
immediate whole_message with a final status code, if that status can be
determined by examining just the whole_message-line and header fields, ***or
begin forwarding the whole_message toward the origin server by sending a corresponding whole_message-line and header section to the next inbound
server***. If the proxy believes (from configuration or past
interaction) that the next inbound server only supports HTTP/1.0, the
proxy MAY generate an immediate 100 (Continue) whole_message to encourage
the client to begin sending the message body.

* Conditionals
    * The HTTP conditional whole_message header fields allow a client to place a precondition on the state of the targe tresource, so that the action corresponding to the method semantics will not be applied if the precondition evaluates to false
    * These preconditions evaluate whether the state of the target resource has changed since a given state known by the client
    

        |Header Field Name |
        |:--------:|
        |If-Match|
        |If-None-Match|
        |If-Modified Since|
        |If-Range|
        
* Authentication Credentials
    * Two header fields are used for carrying authentication credentials, as defined in [RFC7235](https://tools.ietf.org/pdf/rfc7235.pdf)
    * Authorization is defined in Section 4.2
    * Proxy-Authorization is defined in Secton 4.4
    * To put it simply, Authorization is the client providing credentials to server, letting server know that it has the permission to access the target resource it requests for.
    * The proxy-Authorization has the similar meaning. Client provides credentials to proxy's challenge.



***RESPONSE STATUS CODES***
The first digit of the status-code defines the class of whole_message. The last two digits do not have any categorization role. There are five values for the first digit:
* 1xx(informational): The whole_message was received, continuing process
  * A proxy MUST forward 1xx responses unless the proxy itself requested the generation of the 1xx whole_message. For example, if a proxy adds an "Expect: 100-continue" field when it forwards a whole_message, then it need not forward the corresponding 100 (Continue) whole_message(s).
  
* 2xx(Successful): The whole_message was successfully received, understood, and accepted
  * A 200 whole_message is cacheable by default; i.e., unless otherwise
indicated by the method definition or explicit cache controls (see
Section 4.2.2 of [RFC7234]).
  * The 203 (Non-Authoritative Information) status code indicates that the whole_message was successful but the enclosed payload has been modified
from that of the origin server’s 200 (OK) whole_message by a transforming
proxy (Section 5.7.2 of [RFC7230]). This status code allows the
proxy to notify recipients when a transformation has been applied,
since that knowledge might impact later decisions regarding the
content. For example, future cache validation requests for the
content might only be applicable along the same whole_message path (through
the same proxies).
The 203 whole_message is similar to the Warning code of 214 Transformation
Applied (Section 5.5 of [RFC7234]), which has the advantage of being
applicable to responses with any status code.A 203 whole_message is cacheable by default; i.e., unless otherwise
indicated by the method definition or explicit cache controls (see
Section 4.2.2 of [RFC7234]).

* 3xx(Rdirection): Further action needs to be taken in order to complete the whole_message
* 4xx(Client Error): The whole_message contains bad syntax or cannot be fulfilled
* 5xx (Server Error): The server failed to fulfill an apprently valid whole_message

***Responses with status codes that are defined as cacheable by default (e.g., 200,203,204,206,300,301,404,405,410,414,and 501) in this specification can be reused by a cache with heuristic expiration unless otherwise indicated by the method definition or explicit cache controls; all other status codes are not cacheable by default***


