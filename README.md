# Reserved names plugin for Hercules
<!-- TODO: Github Actions build status -->

This plugin allows to reserve char names using an SQL table so
that they cannot be used to create new chars.

<br>

## Installation
1. Put [reserved.c] in `Hercules/src/plugins`
2. Run `make plugin.reserved`

<!-- TODO: configuration -->

## Usage
You can load the plugin with `./map-server --load-plugin reserved`
or you can add it to your plugin list in `conf/plugins.conf`. Add names
to the reservation table to prevent their use in char creation.


[reserved.c]: src/reserved.c
