#!/bin/sh
#
# GPackager Build Script
# (C) 2013 Christian Gunderman
# ..because I was too lazy to build a makefile
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact Email: gundermanc@gmail.com 
#
gcc -std=c99 -g gpac.c ll.c main.c -o gpac
