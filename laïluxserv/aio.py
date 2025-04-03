from fasthtml.common import *
from dataclasses import dataclass


@dataclass
class LuxLevel : k:int; v:int

@dataclass
class LedState : 
    k:int; v:int
    def __ft__(moi):
        if (moi.v == 1) :
            sauce= "/img/lightbulb_on.png"
        else :
            sauce="/img/lightbulb_off.png"
        
        return Img(src=sauce, style="width: 20%")

app = FastHTML(exts='ws')
rt  = app.route
luxdb = database("lux.db")

luxlevel = luxdb.create(LuxLevel, pk="k")
ledstate = luxdb.create(LedState, pk="k")

luxlevel.insert(LuxLevel(v=1))
ledstate.insert(LedState(v=1))

val = 0

head =   Head(
    Title("ioc awesome webpage!"),
    Meta(charset="UTF-8"),
    Meta(name="viewport",content="width=device-width, initial-scale=1.0")
)
body = Body(
    Div(
      # H1("ioc awesome page!"),
      Div(
        Img(src="/img/iocawesomesite.png", style="width: 100%"),
      ),
      Div(
        # H2("DJ !"),
        Img(src="/img/dj.png", style="width: 100%"),
        Select(
          Option("Megalovania - Undertale", value="1"),
          Option("some other song", value="2"),
          Option("yet another song", value="3"),
          size=3
        )
      ),
      Div(
        Div(
          # H2("lux"),
          Img(src="/img/lux.png", style="width: 100%"),
        ),
        Div(
          # H3("Led State"),
          Img(src="img/ledstate.png", style="width: 100%"),
          Div(ledstate(where="k=1")[0], id="led-state"),
          style="height: 50%; width: 50%; float:left"
        ),
        Div(
          # H3("is it day yet ?"),
          Img(src="img/isitdayyet.png", style="width: 100%"),
          P("in function of the photoresistence value"),
          style="height: 50%; width: 50%; float:left"
        )
      ),
      cls="container"
    )
  )

style = Style(
  """
  @font-face {
    font-family: 'Comic Sans MS';
    src: url('/fonts/comic-sans.woff') format('woff');
    font-weight: normal;
    font-style: normal;
  }
  body {
        font-family: 'Comic Sans MS', cursive, sans-serif;
        display: flex;
        flex-direction: column;
        justify-content: flex-start;
        align-items: center;
        min-height: 100vh;
        background-color: #f4f4f4;
        padding-top: 20px;
    }
  .container {
        background: white;
        padding: 20px;
        border-radius: 10px;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        width: 400px;
        text-align: center;
        margin: 10px 0;
    }
    select {
        width: 100%;
        padding: 10px;
        border: 1px solid #ccc;
        border-radius: 5px;
        font-size: 16px;
        overflow: hidden;
        -webkit-appearance: none;
        -moz-appearance: none;
        appearance: none;
        background: linear-gradient(135deg, #ff9a9e, #fad0c4);
        color: white;
        font-weight: bold;
        cursor: pointer;
    }
    select option {
        font-family: 'Comic Sans MS', cursive, sans-serif;
        background: white;
        color: black;
        padding: 10px;
        font-size: 16px;
    }
    select option:hover, select option:checked {
        font-family: 'Comic Sans MS', cursive, sans-serif;
        background: #ff9a9e;
        color: white;
        font-weight: bold;
    }
  """
)

@app.get("/{fname:path}.{ext:static}")
def static(fname:str, ext:str): return FileResponse(f'{fname}.{ext}')

@rt("/")
def get():
    return Main(head, body, style, hx_ext='ws', ws_connect='/ws')

users = {}
def on_conn(ws, send): users[str(id(ws))] = send
def on_disconn(ws): users.pop(str(id(ws)), None)

@app.ws('/ws', conn=on_conn, disconn=on_disconn)
async def ws(op:str, payload: str) :
    try :
        payload = int(payload)
    except ValueError:
        return
    
    if op == 'ledstate':
        ledstate.update(LedState(k=1, v=payload))
    elif op == 'luxlevel' :
        luxlevel.update(LuxLevel(k=1, v=payload))
    else :
        return
    
    for u in users.values(): 
        await u(Div(ledstate(where="k=1")[0], id="led-state"))

serve()
