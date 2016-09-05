Minit
=====

Minit is a minimalist `init` implementation designed for use inside containers,
for instance as the root process in a [Docker](https://www.docker.io/) image.

<https://github.com/chazomaticus/minit>

Use
---

There are three ways to start using minit inside your container.  The easiest,
assuming you're using Docker, is to simply use one of the [minit base
images](https://registry.hub.docker.com/u/chazomaticus/minit/) as a base for
your own Docker image.  See the `example` directory.

If your container will be running Ubuntu, you can also make use of the [minit
PPA](https://launchpad.net/~chazomaticus/+archive/minit) to avoid building
minit yourself.  For another Docker example:

    # This is roughly equivalent to add-apt-repository ppa:chazomaticus/minit.
    RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E007F6BD
    RUN echo "deb http://ppa.launchpad.net/chazomaticus/minit/ubuntu quantal main" > /etc/apt/sources.list.d/minit.list
    
    RUN apt-get update && apt-get upgrade -y && apt-get install -y minit # etc.

Lastly, you can simply build minit by running `make` (or otherwise compiling
the `.c` file into an executable).  Put the resulting executable in your
container's filesystem and set it to run as the root process when your
container starts.

Note that in Docker, you need to use an *exec* form of `ENTRYPOINT` or `CMD`,
as opposed to a *shell* form to run minit.  See for example the [`ENTRYPOINT`
docs](http://docs.docker.com/reference/builder/#entrypoint).  The minit base
images set this up for you automatically.

Operation
---------

When minit starts up, it `exec`s `/etc/minit/startup`, then goes to sleep
`wait`ing on children until it gets a `SIGTERM` (or `SIGINT`, so you can stop
your container with `Ctrl+C` if you ran it in the foreground).  When it gets
one of those signals, it `exec`s `/etc/minit/shutdown` and waits for it to
finish, then `kill`s all remaining processes with `SIGTERM`.

You can override the startup and shutdown scripts with command line arguments:
the first, if non-empty, is run instead of `/etc/minit/startup`, and the second
is run instead of `/etc/minit/shutdown`.  Minit's environment is passed
unmolested to the scripts, so if you need to pass arguments to them, do it
environmentally.

Justification
-------------

If you need more than one process inside your container, it's important for the
root process to behave like `init(8)`.  See
[baseimage-docker](http://phusion.github.io/baseimage-docker/) for a thorough
explanation of what that means and the special considerations for `init` inside
Docker.  Minit solves the init problem in a dead-simple way.

Docker [recommends](http://docs.docker.com/articles/using_supervisord/) running
[Supervisor](http://supervisord.org/) as your container's root process if you
need to start multiple services inside a single container.  Sometimes, though,
you don't want the overhead of a full Python stack tagging along with your nice
clean container image.

*Advantages vs. Supervisor:*
 * No dependencies
 * Smaller (only about 6K in size vs. about 18M for Python and Supervisor
   reported by apt-get on a clean Trusty image)
 * Allows arbitrary commands in container startup and shutdown
 * Easier to control daemons that can't run in the foreground like Postfix

*Disadvantages vs. Supervisor:*
 * Doesn't monitor or restart services

*Advantages vs. baseimage-docker:*
 * Simpler to use
 * Easier to control exactly which services get started
 * Smaller (a Trusty image with minit added is about 275M vs. about 346M for
   baseimage-docker)
 * Works on any distro, not just Ubuntu

*Disadvantages vs. baseimage-docker:*
 * Doesn't automatically run all the services they deem essential


Enjoy!

Thanks to [Arachsys init](https://github.com/arachsys/init) for ideas and
inspiration.
