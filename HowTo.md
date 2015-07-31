# Check out the Heartbeats repository #

Checking out the Heartbeats repository creates a working copy of it on your local machine.

> `svn checkout https://heartbeats.googlecode.com/svn/trunk/ heartbeats --username <YourUserName>`

More information available here: http://code.google.com/p/heartbeats/source/checkout

# Basic Work Cycle #

This section is inspired to the Version Control with Subversion  webpages

Subversion has numerous features, options, bells and whistles, but on a day-to-day basis, odds are that you will only use a few of them. In this section we'll run through the most common things that you might find yourself doing with Subversion in the course of a day's work.

The typical work cycle looks like this:

  * Update your working copy
    * svn update

  * Make changes
    * svn add
    * svn delete
    * svn copy
    * svn move

  * Examine your changes
    * svn status
    * svn diff

  * Possibly undo some changes
    * svn revert

  * Resolve Conflicts (Merge Others' Changes)
    * svn update
    * svn resolved

  * Commit your changes
    * svn commit


# Subversion: a compete guide #
Following this URL (http://svnbook.red-bean.com/index.en.html) you'll be redirected to the online home of Version Control with Subversion, a free book about Subversion, a new version control system designed to supplant CVS.