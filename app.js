'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  console.log("SHIT");
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/someendpoint', function(req , res){
  res.send({
    foo: "bar"
  });
});


let GEDCOMlib = ffi.Library('./GEDCOMlib', {

//put all the stuff that uses the c stuff in here

  'JSONfileNamesToInfo':['string', ['string']],

  'JSONfileToGen':['string', ['string', 'string', 'string', 'int', 'int']],

  'JSONtoGEDCOMfile':['string', ['string', 'string']],

  'JSONtoAddIndi':['string', ['string', 'string']],

  'fileNameToIndiList':['string', ['string']]

});

app.get('/uploading', function(req, res){
  console.log('here?');
  const uploadsFolder = './uploads/';
  var filesJason = "[";

  fs.readdir(uploadsFolder, (err, files) => {
    files.forEach(file => {
        filesJason = filesJason + "{\"fileName\":\"" + file + "\"},";
    });
    
    filesJason = filesJason.substring(0, filesJason.length -1);
    filesJason = filesJason + "]";

    var fileView = GEDCOMlib.JSONfileNamesToInfo(filesJason);

    var jason = JSON.parse(fileView);
    console.log(fileView);
    res.send(jason);


  });

});

app.get('/desc', function(req , res){
  console.log ("HELLO");
  var jsonList;
console.log("fileName is " + req.query.fileName);
  var descList = GEDCOMlib.JSONfileToGen(req.query.fileName, req.query.indiGivn, req.query.indiSurn, req.query.numGen, 0);

  jsonList = JSON.parse(descList);

  console.log(jsonList);
  res.send(jsonList);

});

app.get('/ance', function(req , res){

  var jsonList;

  var descList = GEDCOMlib.JSONfileToGen(req.query.fileName, req.query.indiGivn, req.query.indiSurn, req.query.numGen, 1);

  jsonList = JSON.parse(descList);

  console.log(jsonList);
  res.send(jsonList);

});

app.get('/create', function(req , res){


  console.log("made it to making file");
  console.log("fileName is " + req.query.fileName);
  console.log("source is " + JSON.stringify(req.query.info));
  var err = GEDCOMlib.JSONtoGEDCOMfile(req.query.fileName, JSON.stringify(req.query.info));

  console.log(err);
  res.send(JSON.parse(err));

});

app.get('/addIndi', function(req , res){


  console.log("made it to adding indi");
  console.log("fileName is " + req.query.fileName);
  console.log("info is " + JSON.stringify(req.query.info));
  var err = GEDCOMlib. JSONtoAddIndi(req.query.fileName, JSON.stringify(req.query.info));

  console.log(err);
  res.send(JSON.parse(err));

});

app.get('/indiList', function(req, res){

  var err = GEDCOMlib.fileNameToIndiList(req.query.fileName);

  console.log(err);

  res.send(JSON.parse(err));

});

var username;

var password;

var database;

var connection;

app.get('/login', function(req, res){

  const mysql = require('mysql');

  username = req.query.username;
  password = req.query.password;
  database = req.query.database;

  connection = mysql.createConnection({
    host     : 'dursley.socs.uoguelph.ca',
    user     : username,
    password : password,
    database : database
  });

  connection.connect(function(err){
    if(err){
      console.log("ooooo");
      res.send(JSON.parse("{\"error\":\"yes\"}"));
    }
    else{
      console.log("we good ????");

      connection.query("create table FILE (file_id int not null auto_increment, file_name VARCHAR(60) NOT NULL, source VARCHAR(250), version VARCHAR(10), encoding VARCHAR(10) NOT NULL, sub_name VARCHAR(61), sub_addr VARCHAR(256), num_individuals int, num_families int, primary key(file_id))", function(err, rows, fields){
      if(err){
        console.log("something bad with making table");
      }
      else{
        console.log("all good with making table");
      }
      });

      connection.query("create table INDIVIDUAL (ind_id int not null auto_increment, surname VARCHAR(256) NOT NULL, given_name VARCHAR(256), sex VARCHAR(1), fam_size int, source_file int, primary key(ind_id), foreign key(source_file) references FILE(file_id) ON DELETE CASCADE)", function(err, rows, fields){
        if(err){
          console.log("something bad with making ind table");
        }
        else{
          console.log("all good with making ind table");
        }
      });

      res.send(JSON.parse("{\"error\":\"none\"}"));

    }
  }); 

});

app.get('/storeData', function(req, res){

  console.log('helloooooo');
  const uploadsFolder = './uploads/';
  var filesJason = "[";

  fs.readdir(uploadsFolder, (err, files) => {
    files.forEach(file => {
        filesJason = filesJason + "{\"fileName\":\"" + file + "\"},";
    });
    
    filesJason = filesJason.substring(0, filesJason.length -1);
    filesJason = filesJason + "]"

    var fileView = GEDCOMlib.JSONfileNamesToInfo(filesJason);

    var jason = JSON.parse(fileView);

    connection.query("DELETE FROM INDIVIDUAL", function (err, rows, fields){
      if(err){
        console.log("tried to indis delete, couldn't");
      }
    });

    connection.query("DELETE FROM FILE", function (err, rows, fields){
      if(err){
        console.log("tried to delete files, couldn't");
      }
    });

    var i =1;
    for(let rec of jason){
      if(rec.source != "could not open"){
      var indiStr = GEDCOMlib.fileNameToIndiList(rec.fileName);

      var inds = JSON.parse(indiStr);
      let record = "INSERT INTO FILE (file_id, file_name, source, version, encoding, sub_name, sub_addr, num_individuals, num_families) VALUES ('" + i + "','" + rec.fileName + "','" + rec.source + "','" + rec.gedcVersion + "','" + rec.encoding + "','" + rec.subName + "','" + rec.subAddress + "','" + rec.indiLength + "','" + rec.famLength + "')";

      connection.query(record, function (err, rows, fields){
        if(err){
          console.log("ahh jees");
        }
        else{
        }
      });

      for(let per of inds.list){
        let record = "INSERT INTO INDIVIDUAL ( surname, given_name, sex, fam_size, source_file) VALUES ('" + per.surname + "','" + per.givenName + "','" + per.sex + "','" + per.famSize + "','" + i + "')";
        connection.query(record, function (err, rows, fields){
          if(err){
            console.log("ahh jees");
            console.log(err);
          }
        });
      }
      ++i;
      }
    }

    var returnJason = JSON.parse(filesJason);

    res.send(returnJason);

  });

});

app.get('/clearData', function(req, res){

  console.log("noice");

  connection.query("DELETE FROM INDIVIDUAL", function (err, rows, fields){
    if(err){
      console.log("tried to indis delete, couldn't");
    }
  });
  connection.query("DELETE FROM FILE", function (err, rows, fields){
    if(err){
      console.log("tried to delete files, couldn't");
    }
    else{
      res.send({error: "none"});
    }
  });

});

app.get('/displayDBfiles', function(req, res){

  console.log("displaying db");
  var files;

  connection.query("SELECT COUNT(*) as \"hi\" from FILE", function(err, rows, fields){
    if(err){
      console.log("what");
    }
    else{
      files = rows[0].hi;
      console.log(files);
      res.send(JSON.parse('{"files":"' + files + '"}'));
    }
  });

});

app.get('/displayDBinds', function(req, res){

  var inds;

  connection.query("SELECT COUNT(*) as \"hi\" from INDIVIDUAL", function(err, rows, fields){
    if(err){
      console.log("what");
    }
    else{
      inds = rows[0].hi;
      console.log(inds);
      res.send(JSON.parse('{"inds":"' + inds + '"}'));
    }
  });

});

app.get('/displayByLastName', function(req, res){

  console.log("displaying by lastname");

  connection.query("SELECT * FROM INDIVIDUAL ORDER BY surname", function (err, rows, fields) {
    if(err){
      console.log(err);
      res.send(JSON.parse("{\"error\":\"yes\"}"));
    }
    else{
      console.log("a nice");
      res.send(rows);
    }

  });

});

app.get('/indisByFile', function(req, res){

  console.log("displaying by file" + req.query.fileName);

  var que = 'select * from INDIVIDUAL, FILE where (INDIVIDUAL.source_file = FILE.file_id AND FILE.file_name = "' + req.query.fileName + '")';

  connection.query(que, function (err, rows, fields) {
    if(err){
      res.send({errr : "yes"});
    }
    else{
      res.send(rows);
    }
  });

});

app.get('/anyQuery', function(req, res){

  console.log("getting any query");

  connection.query(req.query.que, function (err, rows, fields) {
    if(err){
      res.send({err : "yes"});
    }
    else{
      res.send({err : "no" , data : rows});
    }
  });

});

app.get('/displayofSurname', function(req, res){

  console.log("displaying of lastname");

  connection.query('SELECT * FROM INDIVIDUAL where (INDIVIDUAL.surname = "' + req.query.surname + '"AND INDIVIDUAL.sex = "' + req.query.sex + '") order by given_name', function (err, rows, fields) {
    if(err){
      console.log(err);
      res.send(JSON.parse("{\"error\":\"yes\"}"));
    }
    else{
      console.log("a nice");
      res.send(rows);
    }

  });

});

app.get('/helpIND', function(req, res){

  connection.query("describe INDIVIDUAL", function (err, rows, fields) {
    if(err){
      res.send({err : "yes"});
    }
    else{
      res.send({err : "no" , data : rows});
    }
  });

});

app.get('/helpFILE', function(req, res){

  connection.query("describe FILE", function (err, rows, fields) {
    if(err){
      res.send({err : "yes"});
    }
    else{
      res.send({err : "no" , data : rows});
    }
  });

});

app.get('/avgNumIndi', function(req, res){

  connection.query("select avg(num_individuals) as \"hello\" from FILE", function (err, rows, fields) {
    if(err){
      res.send({err : "yes"});
    }
    else{
      res.send({err : "no" , data : rows});
    }
  });

});

app.get('/indiWithFile', function(req, res){

  console.log("hello");

  connection.query("select given_name, surname, file_name, source, sub_name from INDIVIDUAL, FILE where (INDIVIDUAL.source_file = FILE.file_id)", function (err, rows, fields) {
    if(err){
      res.send({err : "yes"});
      return;
    }
    else{
      res.send({err : "no" , data : rows});
      return;
    }
  });

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


