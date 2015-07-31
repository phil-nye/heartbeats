# README file for the Application Heartbeats Project (Heartbeats) #

Heartbeats official website:

> http://groups.csail.mit.edu/carbon/heartbeats/

The latest release of Heartbeats is always available in two alternative formats from:

> heartbeatsapi-VX.Y.tar.gz

> heartbeatsapi-VX.Y.zip

You can download the latest release of Heartbeats from:

> http://code.google.com/p/heartbeats/downloads/list

There is a google group for discussion about the development of Heartbeats at

> http://groups.google.com/group/heartbeatsapi

Please read the NEWS file if you are upgrading from a previous release.

Please read the TODO file if you are interested in being informed on the ongoing works. You can also find a list of issues at:

> http://code.google.com/p/heartbeats/issues/list

The contents of this README file are:

  * Project Description
  * Getting Started
  * Directory Organization
  * Documentation for Heartbeats
  * Testing Heartbeats
  * Team Members


## Project Description ##

The Application Heartbeats framework provides a simple, standardized way for applications to monitor their performance and make that information available to external observers. The framework allows programmers to express their application’s goals and the progress that it is making using a simple API. This progress can then be observed by either the application itself or an external system (such as the OS or another application) so that the application or system can be adapted to make sure the goals are met.


## Getting Started ##

  * You can download the latest release of Heartbeats from:
> http://code.google.com/p/heartbeats/downloads/list
  * Once you have downloaded your copy of the Heartbeats framework you just have to unzip it:
> > `unzip heartbeatsapi-VX.Y.zip`

> or:
> > `tar -xzvf heartbeatsapi-VX.Y.tar.gz`
  * At this point you are ready to start to use the Heartbeats framework. You can:
    1. Take a look to the documentation (see "Documentation for Heartbeats")
    1. Familiarize yourself with the Heartbeats API using our examples (see "Testing Heartbeats")
    1. Start to use the Application Heartbeats into your own applications
    1. Get in touch with us and let us know your feedbacks, criticisms, suggestions (send us an email or join our Heartbeats google group at: http://groups.google.com/group/heartbeatsapi)


## Directory Organization ##

/

|-doc/    -- Documentation


> |-html  -- Documentation in HTML format

> |-latex -- Documentation in latex format

|-src/    -- Source files

|-inc/    -- Header files


## Documentation for Heartbeats ##

The documentation for the Heartbeats framework has been created with Doxygen.
You can find both an HTML and Latex version of the documentation in the latest release of the Heartbeats framework.

If, for any reasons, you need to recreate the documentation by yourself, you just have to use the following command (use it from the root directory of the Heartbeats framework):

> `doxygen heartbeats_doc`

Or, if you prefer, you can just use:

> `make documentation`

## Testing Heartbeats ##

The latest release of the Heartbeats framework come with a set of available tests that can be used to familiarize yourself with this API.

You can run one of our two examples, using the following command (use it from the root
directory of the Heartbeats framework):

> `make bench-tp `

to use the throughput example, or

> `make bench-lat`

to use the latency example


## Team Members ##

  * Henry Hoffmann
  * Jonathan Eastep
  * Marco D. Santambrogio
  * Jason E. Miller
  * Anant Agarwal