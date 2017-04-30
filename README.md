# ds3armor
Dark Souls 3 armor optimizer

An extesion of the idea proposed [here](https://www.reddit.com/r/darksouls3/comments/682wel/the_best_armor_combinations_based_on_absorption/) by [/u/mound_maker](https://www.reddit.com/user/mound_maker). Armor data obtained from [Mugenmonkey](https://mugenmonkey.com/).

In the original topic a number of top tier armor pieces were put together to form a top tier armor set, sorted by weight. This program calculates all possible combinations of all armor pieces and sorts them by weight, then by an user-defined "score" based on absorption.

## The set scoring
A set is scored by its absorption values, averaged together with some user-specified weights. The weight can be used to take into account how much damage you expect to take for a specific elemental damage, after the [calculation of your defense stat](https://www.reddit.com/r/darksouls3/comments/4f8yy8/how_defense_and_absorption_really_work/). You can estimate for example that after defense you get a physical damage 3 times larger than the magic damage from a magic buffed sword, then you probably want to weight physical damage three times more than magic damage. This is up to you and your experience as a Dark Souls 3 player.

### Weight specification

These are the arguments to modify the weights for each absorption:
```
  --physical arg      physical absorption weight
  --vs_strike arg     vs_strike absorption weight
  --vs_slash arg      vs_slash absorption weight
  --vs_thrust arg     vs_thrust absorption weight
  -m arg              magic absorption weight
  -f arg              fire absorption weight 
  -l arg              lightning absorption weight
  -d arg              dark absorption weight
```

The default phyisical damage weights are 0.25 add up to 1, while the default elemental damage weights are 0. So, if you want to specify that magic damage should be accounted for 30% of its value against physical damage, you specify `-m 0.3`.

### Balanced or unbalanced averages

There are two possible averages:

* balanced average (`--balanced 1`, the default): use harmonic averages, in order to penalize sets with a specific weakness
* unbalanced average (`--balanced 0`): use normal averages, weaknesses of a set are less likely to penalize the set

A versatile build probably needs `--balanced 1`.

## Precompiled lists

I generated a number of sensible default with this program in the [standard-lists](https://github.com/pisto/ds3armor/tree/master/standard-lists) folder.

* pure_{physical,fire,magic,lightning,dark}: score based only on {physical,fire,magic,lightning,dark}
* {fire,magic,lightning,dark,mixed}_{weight}: physical (weight 1) averaged with elemental at some specified weight between 0.1 and 0.9 - mixed is an average of all elemental damages with the same weights
* {fire,magic,lightning,dark}_unbalanced_{weight}: same as above but with `--balanced 0`

## Download program
Windows x64 builds are available [here](https://github.com/pisto/ds3armor/releases). You need a processor with the AVX instruction set to run the build (most processors from 2012 should work), otherwise you'll need to recompile from source.

## Building the program
The build depends on GCC and [Boost.Program_options](http://www.boost.org/doc/libs/1_64_0/doc/html/program_options.html). `make` should suffice in most occasions. To cross compile specify your compiler program with `make CXX=compiler`.
