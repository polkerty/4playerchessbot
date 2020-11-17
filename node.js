const express = require('express')

Board = require("/Users/jacobbrazeal/Desktop/experiments/4pc/bot.js").Board;

var board = new Board(0);
board.ab_width = [3, 2, 1, 1];
board.ab_depth = 5;
//console.log(board.ordered_moves(board.next_move));


const app = express()
const port = 3000

app.get('/', (req, res) => res.send('Chess chess chess chess server, oh yeah'))
app.use(express.json({extended:true}))
app.post('/', function (req, res) {
    
    board.fromState(req.body);
    console.log(board.ab_recommended_move(board.next_move));
    res.send(board.ab_recommended_move(board.next_move))
})



app.listen(port, () => console.log(`Chess chess chess chess bot live on port ${port}!`))