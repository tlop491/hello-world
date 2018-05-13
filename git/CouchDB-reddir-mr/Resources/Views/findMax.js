{
  "views": {
    "all": {
      "map": "function(doc) { emit([doc.body], [doc.score]) }"
            
      "reduce": "function (key, values, rereduce) {\n    var max = -Infinity\n    for(var i = 0; i < values.length; i++)\n        if(typeof values[i] == 'number')\n            max = Math.max(values[i], max)\n    return max\n}"

    }
  }
}
