#! /bin/sh

./bin/lenia-toybox              \
-w 200                          \
-h 150                          \
-f 60                           \
-a lenia                        \
-r 10                           \
-t 10                           \
--kernel-count 3                \
                                \
--growth-center-1 0.156         \
--growth-width-1 0.0118         \
--ring-weight-1-1 1.0           \
--ring-weight-1-2 0.416         \
--ring-weight-1-3 0.667         \
                                \
--growth-center-2 0.193         \
--growth-width-2 0.049          \
--ring-weight-2-1 0.083         \
--ring-weight-2-2 1.0           \
                                \
--growth-center-3 0.342         \
--growth-width-3 0.089          \
--ring-weight-3-1 1.0           \
                                \
-s startups/fish.bmp            \
