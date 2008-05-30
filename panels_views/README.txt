/* $Id: README.txt,v 1.1.2.1 2008/05/30 00:46:33 merlinofchaos Exp $ */

Panels 2 ships with two new modules dealing with Views: panels_views and
panels_views_legacy.

In alpha versions of Panels 2 (and in Panels 1.x), all views were automatically
available as content for panels - and there were a relatively complex set of
settings in the "add content" form to add a view (arguments, type, read more
link, all that stuff).

The panels_views module changes this. Instead of all views automatically showing
up, you go to admin/panels/views - and for each view you want to make available,
you set up how it will be presented in the panels interface, plus defaults. This
simplifies the panels interface itself (meaning that admins, editors or whoever
can much more easily add views to their panels without all those complex options
muddying the form) - but means more initial work before any view can be added.
Don't forget to give yourself the additional permissions this module creates if
you're not logged on as user/1!

If you already have panels from a previous version set up with views and simply
run update.php, ALL OF YOUR VIEWS WILL DISAPPEAR FROM YOUR PANELS - unless you
enable the panels_views_legacy module. This module simply allows you to add
views in exactly the same way as you could in alpha, and will leave all your
current panels intact. So if your panels are all broken, and you've arrived
here, just enable the module and take a deep breath.

Also note that you can have both modules enabled at the same time - this will
keep your old panel setups intact while you configure your views for the new
method. As you configure your panels_views, unless you change the defaults when
configuring them they will show up in the add content form with exactly the same
name appearing as a duplicate. If you have a large number of views it may be
worth changing the title or description in order to differentiate during this
process.

