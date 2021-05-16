#!flask/bin/python
from flask import Flask, jsonify, request, abort
from datetime import datetime, timedelta
from flask import Flask, redirect, render_template, request, jsonify, abort, flash
from flask_sqlalchemy import SQLAlchemy, Model
from sqlalchemy import desc ,asc
from flask_marshmallow import Marshmallow
from marshmallow import Schema, fields, pprint
from datetime import datetime, timedelta
import  os
from os.path import isfile, join
from os import listdir
import json
from io import StringIO
from werkzeug.wrappers import Response
import itertools
import random
import string


app = Flask(__name__)
app.secret_key = 'development key'

SQLALCHEMY_DATABASE_URI = "mysql+mysqlconnector://{username}:{password}@{hostname}/{databasename}".format(
    username="felixl120",
    password="alamakota", # database passowrd hidden
    hostname="felixl120.mysql.eu.pythonanywhere-services.com",
    databasename="felixl120$test",
)
app.config["SQLALCHEMY_DATABASE_URI"] = SQLALCHEMY_DATABASE_URI
app.config["SQLALCHEMY_POOL_RECYCLE"] = 299 # connection timeouts
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False # no warning disruptions

db = SQLAlchemy(app)
ma = Marshmallow(app)


class Roslina(db.Model):

    __tablename__ = "roslina"
    id = db.Column(db.Integer, primary_key=True)
    nazwa = db.Column(db.String(4096))
    gatunek = db.Column(db.String(4096))
    czas_dodania = db.Column(db.DATETIME)



    def __init__(self, nazwa, gatunek, czas_dodania):
        self.nazwa = nazwa
        self.gatunek = gatunek
        self.czas_dodania = czas_dodania

class RoslinaSchema(ma.Schema):
    class Meta:
        # Fields to expose
        fields = ('id' ,'nazwa', 'gatunek','czas_dodania')



class Pomiary(db.Model):

    __tablename__ = "pomiary"
    id = db.Column(db.Integer, primary_key=True)
    roslina = db.Column(db.Integer)
    naslonecznienie = db.Column(db.Float)
    temperatura = db.Column(db.Float)
    nawodnienie = db.Column(db.Float)
    czas_dodania = db.Column(db.DATETIME)



    def __init__(self, roslina, naslonecznienie, temperatura, nawodnienie, czas_dodania):
        self.roslina = roslina
        self.naslonecznienie = naslonecznienie
        self.temperatura = temperatura
        self.nawodnienie = nawodnienie
        self.czas_dodania = czas_dodania

class PomiarySchema(ma.Schema):
    class Meta:
        # Fields to expose
        fields = ('id' ,'roslina', 'naslonecznienie', 'temperatura', 'nawodnienie', 'czas_dodania')


class Czasy_podlania(db.Model):

    __tablename__ = "czasy_podlania"
    id = db.Column(db.Integer, primary_key=True)
    roslina = db.Column(db.Integer)
    data_operacji = db.Column(db.DATETIME)



    def __init__(self, roslina, data_operacji):
        self.roslina = roslina
        self.data_operacji = data_operacji

class Czasy_podlaniaSchema(ma.Schema):
    class Meta:
        # Fields to expose
        fields = ('id' ,'roslina', 'data_operacji')

roslina_schema = RoslinaSchema()
rosliny_schema = RoslinaSchema(many=True)
pomiar_schema = PomiarySchema()
pomiary_schema = PomiarySchema(many=True)
czas_podlania_schema = Czasy_podlaniaSchema()
czasy_podlania_schema = Czasy_podlaniaSchema(many=True)

@app.route("/rosliny", methods=["GET"])
def get_rosliny():
    all_users = Roslina.query.all()
    result = rosliny_schema.dump(all_users)
    return jsonify(result)

@app.route("/rosliny/<id>", methods=["GET"])
def get_roslina(id):
    roslina = Roslina.query.get(id)
    if bool(roslina)==False:
       abort(404)
    result = roslina_schema.dump(roslina)
    return jsonify(result)

@app.route("/rosliny", methods=["POST"])
def add_roslina():
    nazwa= request.json["nazwa"]
    gatunek= request.json["gatunek"]
    new_roslina = Roslina(nazwa, gatunek, datetime.now())
    db.session.add(new_roslina)
    db.session.commit()
    roslina = Roslina.query.get(new_roslina.id)
    return roslina_schema.jsonify(roslina)

@app.route("/rosliny/<id>", methods=["PUT"])
def change_roslina(id):
    roslina = Roslina.query.get(id)
    if len(roslina.nazwa) == 0:
        abort(404)
    if not request.json:
        abort(400)
    roslina.nazwa = request.json.get('nazwa', roslina.nazwa)
    roslina.gatunek = request.json.get('gatunek', roslina.gatunek)
    db.session.commit()
    return roslina_schema.jsonify(roslina_schema.dump(roslina))



@app.route("/pomiary", methods=["GET"])
def get_pomiary():
    all_pomiary = Pomiary.query.all()
    result = pomiary_schema.dump(all_pomiary)
    return jsonify(result)

@app.route("/pomiary/<id>", methods=["GET"])
def get_pomiar(id):
    pomiar = Pomiary.query.get(id)
    if bool(pomiar)==False:
       abort(404)
    result = pomiar_schema.dump(pomiar)
    return jsonify(result)

@app.route("/pomiary", methods=["POST"])
def add_pomiar():
    roslina= request.json["roslina"]
    naslonecznienie= request.json["naslonecznienie"]
    nawodnienie= request.json["nawodnienie"]
    temperatura= request.json["temperatura"]
    new_pomiar = Pomiary(roslina, naslonecznienie, temperatura, nawodnienie, datetime.now())
    db.session.add(new_pomiar)
    db.session.commit()
    pomiar = Pomiary.query.get(new_pomiar.id)
    return pomiar_schema.jsonify(pomiar)

@app.route("/pomiary/<id>", methods=["PUT"])
def change_pomiar(id):
    pomiar = Pomiary.query.get(id)
    if bool(pomiar)==False:
       abort(404)
    if not request.json:
        abort(400)
    pomiar.roslina = request.json.get('roslina', pomiar.roslina)
    pomiar.naslonecznienie = request.json.get('naslonecznienie', pomiar.naslonecznienie)
    pomiar.temperatura = request.json.get('temperatura', pomiar.temperatura)
    pomiar.nawodnienie = request.json.get('nawodnienie', pomiar.nawodnienie)
    db.session.commit()
    return pomiar_schema.jsonify(pomiar_schema.dump(pomiar))


@app.route("/czasy", methods=["GET"])
def get_czasy():
    all_czasy = Czasy_podlania.query.all()
    result = czasy_podlania_schema.dump(all_czasy)
    return jsonify(result)

@app.route("/czasy/<id>", methods=["GET"])
def get_czas(id):
    czas = Czasy_podlania.query.get(id)
    if bool(czas)==False:
       abort(404)
    result = czas_podlania_schema.dump(czas)
    return jsonify(result)

@app.route("/czasy", methods=["POST"])
def add_czas():
    roslina= request.json["roslina"]
    data_operacji=request.json["data_operacji"]
    new_czas = Czasy_podlania(roslina, data_operacji)
    db.session.add(new_czas)
    db.session.commit()
    czas = Czasy_podlania.query.get(new_czas.id)
    return czas_podlania_schema.jsonify(czas)

@app.route("/czasy/<id>", methods=["PUT"])
def change_czas(id):
    czas = Czasy_podlania.query.get(id)
    if bool(czas)==False:
       abort(404)
    if not request.json:
        abort(400)
    czas.roslina = request.json.get('roslina', czas.roslina)
    czas.data_operacji = request.json.get('data_operacji', czas.data_operacji)
    db.session.commit()
    return czas_podlania_schema.jsonify(czas_podlania_schema.dump(czas))


@app.route('/todo/api/v1.0/time', methods=['GET'])
def get_time():
    return jsonify({'time': datetime.now()})



@app.route('/')
def strona_rosliny():

    all_rosliny=Roslina.query.all()
    result=rosliny_schema.dump(all_rosliny)


    return render_template('podstrona.html',title='IPZ_podlewanie',rosliny=result)

@app.route('/pomiary/lista/<idd>')
def strona_rosliny_pomiary(idd):

    all_rosliny=Roslina.query.all()
    result=rosliny_schema.dump(all_rosliny)

    all_pomiary = Pomiary.query.all()
    res = pomiary_schema.dump(all_pomiary)

    all_czasy = Czasy_podlania.query.all()
    resul = czasy_podlania_schema.dump(all_czasy)

    nrss=int(idd)


    return render_template('pomiary.html',title='IPZ_podlewanie',rosliny=result,pomia=res,czasy=resul,nrs=nrss)



@app.route("/pomiary/szukaj/<r>", methods=["GET"])
def get_pomiarr(r):
    pomiar = Pomiary.query.filter_by(roslina=r).all()
    if bool(pomiar)==False:
       abort(404)
    result = pomiary_schema.dump(pomiar)
    return jsonify(result)



@app.route('/rosliny/dodaj', methods=['GET', 'POST'])
def new_roslina():
    if request.method == 'GET':
        return render_template('nowa_roslina.html',title='IPZ_podlewanie')
    nazwa= request.form["nazwa"]
    gatunek= request.form["gatunek"]
    new_roslina = Roslina(nazwa, gatunek, datetime.now())
    db.session.add(new_roslina)
    db.session.commit()
    return render_template('nowa_roslina.html',title='IPZ_podlewanie')


@app.route('/czasy/dodaj', methods=['GET', 'POST'])
def new_czas():
    if request.method == 'GET':
        return render_template('nowy_czas.html',title='IPZ_podlewanie')
    roslina= request.form["roslina"]
    data_operacji=request.form["data_operacji"]
    new_czas = Czasy_podlania(roslina, data_operacji)
    db.session.add(new_czas)
    db.session.commit()
    return render_template('nowy_czas.html',title='IPZ_podlewanie')
