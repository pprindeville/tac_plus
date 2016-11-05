# Why another copy of tac_plus was (not really) needed...

After trying for a more than a month to get some patches upstreamed,
I've decided that the path of least resistance is to post them here,
and if anyone wants them they can help themselves.

Chasing upstream maintainers is more work than I have energy for.

I used [Heasley's tac_plus](http://shrubbery.net/tac_plus/) to populate
this repo.

## How to build this project

My autotools versions are different enough (more recent) from upstream
that I've not bothered updating the contents of `Makefile`, `Makefile.in`
or `aconf/'.  I did _however_ update `configure`.  That said, you're
better off with a clean slate so please follow these directions when
building (the first one is particularly important):

```
% autoreconf -f -i
% ./configure --enable-acls --enable-uenable --enable-warn \
	      --enable-stderr-timestamps
% make
```

to force the contents of `aconf/` to be refreshed, and `Makefile.in`
to be reconstructed.

## Going forward

I don't intend to become the new maintainer of `tacacs+`.  As I said,
it's easier to let upstream pull my changes than me trying to insist
they pick them up.

If that happens, and they become more responsive about picking up
one-off changes that might get occasionally submitted by me or others,
then I will retire this repo as it will have served its purpose.

In the meantime, however, I will consider useful patches (especially
if they can be compiled out).  To give those submissions the best
chances of being accepted:

* avoid whitespace only changes (reformatting, trimming blanks from
the end of lines, etc.  I don't like these: they make it too hard to
see what's substantively changed at-a-glance;

* stick to the original formatting style;

* make sure your changes conform to widely accepted best practices;

* vet them sufficiently locally so I have some assurance that they
work in a production environment;

* adequately document them (both in the commit header and any
relevant man page changes, etc);

If in doubt, email me.  Best to get on the same page before making
a big effort, only to find it needs to be redone.
