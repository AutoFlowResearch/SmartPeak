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
__copyright__   = "The SmartPeak Team -- Novo Nordisk Foundation Center for Biosustainability, Technical University of Denmark 2018-2021."
__license__     = "MIT"
__version__     = "1.0.0"
__maintainer__  = "Ahmed Khalil"
__status__      = "Production"

"""
Add a unique user to the users database (active_users table).

Usage:
    ./user-add.py <new_user_name>
    
    Then enter a password of 8 characters or more,
    usernames that already exist in the data base won't be replaced
    or updated.
"""

import os, sys
import sqlite3, hashlib
from getpass import getpass

from config import DB_PATH, PASSWORD_MIN_LENGTH, HASH_ALGORITHM

if len(sys.argv) != 2:
    print("USAGE: %s <username>" % sys.argv[0])
    sys.exit(1)
if not os.path.exists(DB_PATH):
    print("ERROR: Database not found: %s" % DB_PATH)

hash_function = getattr(hashlib, HASH_ALGORITHM, None)
if hash_function is None:
    print("ERROR: Hashing algorithm '%s' not found" % HASH_ALGORITHM)
    sys.exit(2)

username = sys.argv[1]
password_matched = False
while not password_matched:
    password = getpass()
    if len(password) < PASSWORD_MIN_LENGTH:
        print("ERROR: password must be at least %d characters long" % PASSWORD_MIN_LENGTH)
        continue
    password_confirm = getpass('Reenter password to confirm: ')
    if password == password_confirm:
        password_matched = True
    else:
        print("ERROR: password does not match")

password = hash_function(password.encode("UTF-8")).hexdigest()

db = sqlite3.connect(DB_PATH)
cursor = db.cursor()
try:
    cursor.execute("INSERT INTO active_users VALUES (?, ?);", (username, password))
except sqlite3.IntegrityError:
    print("ERROR: user '%s' already exists" % username)
    sys.exit(2)
db.commit()

print(">> User %s successfully created" % username)
