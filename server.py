# -*- coding: utf-8 -*-

from flask import Flask, request, jsonify,render_template
from flask_cors import CORS
import mysql.connector

conexao = mysql.connector.connect(
    host="secret",
    database="secret",
    user="secret",
    password="secret",
    port=secret,
)

cursor = conexao.cursor()

app = Flask(__name__)
CORS(app)

  
@app.route('/', methods=['POST', 'GET'])
def rota_do_flask():
    global ultima_resposta_json
    
    if request.method == 'POST':
        parametro = request.form
        ultimavenda = parametro.get('ultima')
        dinheiroacumulado = parametro.get('acumulado')
        metadiaria = parametro.get('metadiaria')
        temperatura = parametro.get('temperatura')
        frasedodia = parametro.get('frasedodia')

        if ultimavenda is not None:
            ultima_resposta_json = {"venda": "ultima"}

        elif dinheiroacumulado is not None:
            ultima_resposta_json = {"acumulado": "dinheiro"}

        elif metadiaria is not None:
            ultima_resposta_json = {"meta": "metadiaria"}

        elif temperatura is not None:
            ultima_resposta_json = {"temp": "temperatura"}

        elif frasedodia is not None:
            ultima_resposta_json = {"frase": "frasedodia"}

        else:
            ultima_resposta_json = {"senha" : "senhagerada"}

        return jsonify(ultima_resposta_json)

    elif request.method == 'GET':
        return jsonify(ultima_resposta_json)

@app.route("/vendas", methods=["POST"])
def vendas():
    if request.method == "POST":
        conexao = mysql.connector.connect(
        host="secret",
        database="secret",
        user="secret",
        password="secret",
        port=secret,
        )

        cursor = conexao.cursor()
        try:
            dados = request.json
            print('Solicitação recebida:', dados)
            
            order_date = dados.get('data', {}).get('purchase', {}).get('order_date')
            price_value = dados.get('data', {}).get('purchase', {}).get('price', {}).get('value')
            try:
              cursor.execute("""
                INSERT INTO displaylcd (hotmart,metaeconomica) 
                VALUES (%s,%s)
            """, (order_date,price_value,))
            
              conexao.commit()
            except Exception as e:
              print("Erro durante a execução:",str(e))

            return jsonify({"mensagem": "Solicitação recebida com sucesso", "order_date": order_date, "price": price_value})
        except Exception as e:
            print("Erro durante a execução:",str(e))
            return jsonify({"erro": str(e)}), 500
    else:
        print("Erro durante a execução:",str(e))
        return jsonify({"erro": "Método não permitido"}), 405


@app.route("/leds",methods=['POST','GET'])
def leds():
  
  global ultima_sequencia_json
  
  if request.method == 'POST':
    parametro = request.get_json()
    print(parametro)
    ultima_sequencia_json = parametro
    print(ultima_sequencia_json)
    return jsonify(ultima_sequencia_json)
  
  elif  request.method == 'GET':
    return jsonify(ultima_sequencia_json)
    
  
      
if __name__ == "__main__":
    app.run(host='secret', port=secret)
