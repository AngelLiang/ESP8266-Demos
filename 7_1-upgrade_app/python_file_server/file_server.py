# coding=utf-8
# FilenName: file_server.py
"""
python file_server.py
URL: http://127.0.0.1:5000/admin/fileadmin/
"""

import os
from flask import Flask
from flask_admin.contrib.fileadmin import FileAdmin
from flask_admin import Admin

app = Flask(__name__)

basedir = os.path.abspath(os.path.dirname(__file__))
path = os.path.join(basedir, 'file')

if not os.path.exists(path):
    os.mkdir(path)

admin = Admin(app, name="FileServer", template_mode="bootstrap3")
admin.add_view(FileAdmin(path, name='file'))

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)
