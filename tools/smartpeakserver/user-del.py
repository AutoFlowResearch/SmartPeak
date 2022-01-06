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
Deletes an existing user from the users (active_users table) database.

Usage:
    ./user-del.py <existing_user_name>
"""

import os, sys
import sqlite3

from config import DB_PATH

if len(sys.argv) != 2:
    print("USAGE: %s <username>" % sys.argv[0])
    sys.exit(1)
if not os.path.exists(DB_PATH):
    print("ERROR: Database not found: %s" % DB_PATH)
    sys.exit(2)

username = sys.argv[1]
db = sqlite3.connect(DB_PATH)
cursor = db.cursor()
cursor.execute("DELETE FROM active_users WHERE username = ?;", (username,))
db.commit()
print(">> User '%s' successfully removed" % username)
