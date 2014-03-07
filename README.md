##Proto Mesh Democracy

This is where I am working on a RetroShare based Implementation of a Liquid Democracy Type system.  
Help is welcome.

The current primary differences between this and "Posted Links" is the additions of "Representation" and a "Topic Tree"

By allowing people to pick their representitive for a topic we should get better results as votes are "weighted by trust"
A Topic Tree will let representation be "passed down" as well as allowing for more self organisation.

The general starting concept is to clone Posted Links, and add a group heirachy with topical representation that can form a representation tree over the topic heirachy

Initial TODO (in approx order):

1. ~~create new blank plugin/service to work in~~
2. ~~Groups - create new RS plugin with its own group (clone posted groups into new plugin)~~
3. ~~ParentGroups - port previous "parentgroups" work into new plugins service and clone of groupwidget~~
4. ~~Posting - clone basic posted links posting system and GUI~~
5. ~~finish (4.) - including to port/enable comment system~~
6. ~~Representation - Add a way to select a representitive for a topic~~
7. Take representation tree into account when counting votes for a post or comment
8. Build Representation Tree Model, data and visual

further TODO:

1. improve security (strong vote checking)
2. caching - lots of it.
3. deal with old representation items (delete?)
4. allow vote changing (same method as representation)
5. Seperate into plugin
