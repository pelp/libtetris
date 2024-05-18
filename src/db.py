#!/bin/env python3
import sqlite3
import datetime
import asyncio
import json
from websockets.server import serve

DBNAME = "leaderboard"

con = sqlite3.connect("leaderboard.db")
cur = con.cursor()

connections = {}

res = cur.execute("SELECT name FROM sqlite_master")
try:
    res = cur.execute(f"CREATE TABLE {DBNAME}(name, time, difficulty, date)")
    print("Created leaderboard")
except sqlite3.OperationalError:
    print("Loading leaderboard")


def post_score(name, time):
    cur.execute(
        f"INSERT INTO {DBNAME} VALUES ('{name}', {time}, 'default', '{datetime.datetime.now()}')")
    con.commit()

def get_scores(difficulty):
    return cur.execute(
        """
        SELECT
            name, MAX(CAST(time AS INTEGER))
        FROM
            {DBNAME}
        WHERE
            difficulty = '{difficulty}'
        GROUP BY
            name
        ORDER BY
            CAST(time AS INTEGER) DESC
        """.format(DBNAME=DBNAME, difficulty=difficulty)
    ).fetchmany(20)

async def handle(websocket):
    connections[websocket] = ""
    async for message in websocket:
        ns = json.loads(message)
        if len(ns) > 1:
            connections[websocket] = 'default'
            post_score(ns[0][:10], ns[1])
            scores = get_scores('default')
            print(scores)
            for c, d in list(connections.items()):
                if d != 'default':
                    continue
                try:
                    await c.send(json.dumps(scores))
                except:
                    del connections[c]
        else:
            connections[websocket] = ns[0]
            await websocket.send(json.dumps(get_scores(ns[0])))
    del connections[websocket]
        

async def main():
    async with serve(handle, "localhost", 8763):
        await asyncio.Future()

asyncio.run(main())

con.close()
