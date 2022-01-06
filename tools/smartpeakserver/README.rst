Users database management tools for SmartPeak Server
====================================================

TL;DR
-----
Follow these steps to easily create a users data base and update it with usernames and passwords

#. Set ``DB_PATH`` to the full path of the user.db you wish to create.
#. Execute ``./create-db.py`` to create the database.
#. Add users by calling ``./user-add.py <NEW_USER_NAME>`` then enter the password and confirm your entry.
#. Delete users by calling ``./user-del.py <EXISTING_USER_NAME>``.
#. List users by calling ``./user-list.py``.