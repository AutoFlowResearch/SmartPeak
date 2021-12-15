#!/usr/bin/env python
# -*- coding: utf-8 -*-
# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation
# Center for Biosustainability, Technical University of Denmark 2018-2021.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

__author__      = "Ahmed Khalil"
__copyright__   = "The SmartPeak Team -- Novo Nordisk Foundation Center for Biosustainability, Technical University of Denmark 2018-2022."
__license__     = "MIT"
__version__     = "1.0.0"
__maintainer__  = "Ahmed Khalil"
__status__      = "Production"

"""
Creates a users database with an 'active_users' table.

Usage:
    ./create-db.py
    
    "DB_PATH" must be set in the `config.py` file to the
    full path of the database including the database suffix "db".
"""

import os, sys
import sqlite3

from config import DB_PATH

if os.path.exists(DB_PATH):
    print("ERROR: File already exists: %s" % DB_PATH)
    sys.exit(2)

db = sqlite3.connect(DB_PATH)
cursor = db.cursor()
cursor.execute("CREATE TABLE active_users (username TEXT NOT NULL UNIQUE PRIMARY KEY, password TEXT NOT NULL);")
print(">> Users database created : %s" % DB_PATH)
