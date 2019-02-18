// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    console.log("before");
    var fileTable = document.getElementById("fileViewTable");
    var status = document.getElementById("statusBox");
    var indiTable = document.getElementById("GEDCOMviewTable")

    var hi = document.getElementById("loginWindow");

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/uploading',  
        success: function (data) {

            for(var i = 0; i < data.length; ++i){
                var row = fileTable.insertRow(i + 1);
                var fileNameCell = row.insertCell(0);
                var sourceCell = row.insertCell(1);
                var versionCell = row.insertCell(2);
                var encodingCell = row.insertCell(3);                   
                var submNameCell = row.insertCell(4);
                var sumbAddressCell = row.insertCell(5); 
                var numIndisCell = row.insertCell(6);
                var numFamsCell = row.insertCell(7);  
                var download = document.createElement('file');
                var fileDownload = "<a href=\"./uploads/"+data[i].fileName+"\">" + data[i].fileName + "</a>";
                fileNameCell.innerHTML = fileDownload;

                sourceCell.innerHTML = data[i].source;
                if(data[i].source != "could not open"){

                    versionCell.innerHTML = data[i].gedcVersion;
                    encodingCell.innerHTML = data[i].encoding;               
                    submNameCell.innerHTML = data[i].subName;
                    sumbAddressCell.innerHTML = data[i].subAddress;
                    numIndisCell.innerHTML = data[i].indiLength;
                    numFamsCell.innerHTML = data[i].famLength;

                    var select1 = document.getElementById("addIndiFileName");
                    select1[select1.length] = new Option(data[i].fileName, data[i].fileName);
                    var select2 = document.getElementById("getDescFile");
                    select2[select2.length] = new Option(data[i].fileName, data[i].fileName);
                    var select3 = document.getElementById("getAnceFile");
                    select3[select3.length] = new Option(data[i].fileName, data[i].fileName);
                    var select4 = document.getElementById("seeFileName");
                    select4[select4.length] = new Option(data[i].fileName, data[i].fileName);

                }
                else{
                    status.innerHTML = status.innerHTML + "failed to open " + data[i].fileName + "<br>";
                }
            }
            if(data.length == 0){
                var row = fileTable.insertRow(1);
                row.innerHTML = "No Files to Display";
            }

        },
        fail: function(error) {
            status.innerHTML = status.innerHTML + "error loading files" + "<br>";
            console.log(error); 
        }
    });

});


$("#seeFile").submit(function(e){

    var status = document.getElementById("statusBox");
    var GEDCOMtable = document.getElementById("GEDCOMviewTable");
    var fileTable = document.getElementById("fileViewTable");

    e.preventDefault();

    for(var j = GEDCOMtable.rows.length -1; j > 0; --j){
        GEDCOMtable.deleteRow(j);
    }

    fileName = document.getElementById("seeFileName").value;

console.log(fileName);

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/indiList',
        data : {
            fileName : fileName,
        },
        success: function (data) {
            if(data.error != "OK"){
                status.innerHTML = status.innerHTML + "Failed to get info for " + fileName + "<br>";
            }
            else{
                status.innerHTML = status.innerHTML + "Got info for " + fileName + "<br>";
                for(var i = 0; i < data.list.length; ++i){
                    var row = GEDCOMtable.insertRow(i + 1);
                    var givn = row.insertCell(0);
                    var surn = row.insertCell(1);
                    var sex = row.insertCell(2);
                    var famSize = row.insertCell(3);
                    givn.innerHTML = data.list[i].givenName;
                    surn.innerHTML = data.list[i].surname;
                    sex.innerHTML = data.list[i].sex;
                    famSize.innerHTML = data.list[i].famSize;  
                }
                if(data.list.length == 0){
                    var row = GEDCOMtable.insertRow(1);
                    row.innerHTML = "No individuals to display";
                }
            }
        },
        fail: function(error) {
            status.innerHTML = status.innerHTML + "Error opening file" + fileName + "<br>";
        }
    });

});

    // Listeners for Section 3
$("#createGED").submit(function(e){

    var status = document.getElementById("statusBox");
    var GEDCOMtable = document.getElementById("GEDCOMviewTable");
    var fileTable = document.getElementById("fileViewTable");

    e.preventDefault();

    var fileName = document.getElementById("createGEDFileName").value;
   // var source = document.getElementById("source").value;
    var info = { "source" : document.getElementById("source").value,
    "gedcVersion" : document.getElementById("gedcVers").value, 
    "encoding" : document.getElementById("encoding").value,
    "subName" : document.getElementById("submName").value,
    "subAddress" : document.getElementById("submAddress").value
    };

    if(info.subAddress.charAt(0) != '@'){
        info.subAddress = "@" + info.subAddress;
    }
    if(info.subAddress.charAt(info.subAddress.length -1) != '@'){
        info.subAddress = info.subAddress + "@";
    }

    var checkin = fileName.substring(fileName.length - 4, fileName.length);
    if(checkin == '.ged'){

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/create',
        data : {
            fileName : fileName,
            info : info,
        },
        success: function (data) {
            if(data.error != "OK"){
                status.innerHTML = status.innerHTML + "Failed to create " + fileName + "<br>";
            }
            else{
                status.innerHTML = status.innerHTML + "Created new file " + fileName + "<br>";
                var row = fileTable.insertRow(fileTable.rows.length);
                var fileNameCell = row.insertCell(0);
                var sourceCell = row.insertCell(1);
                var versionCell = row.insertCell(2);
                var encodingCell = row.insertCell(3);                   
                var submNameCell = row.insertCell(4);
                var sumbAddressCell = row.insertCell(5); 
                var numIndisCell = row.insertCell(6);
                var numFamsCell = row.insertCell(7);
                var fileDownload = "<a href=\"./uploads/"+fileName+"\">" + fileName + "</a>";
                fileNameCell.innerHTML = fileDownload;
                sourceCell.innerHTML = info.source;
                versionCell.innerHTML = info.gedcVersion;
                encodingCell.innerHTML = info.encoding;               
                submNameCell.innerHTML = info.subName;
                sumbAddressCell.innerHTML = info.subAddress;
                numIndisCell.innerHTML = "0";
                numFamsCell.innerHTML = "0";

                var select1 = document.getElementById("addIndiFileName");
                select1[select1.length] = new Option(fileName, fileName);
                var select2 = document.getElementById("getDescFile");
                select2[select2.length] = new Option(fileName, fileName);
                var select3 = document.getElementById("getAnceFile");
                select3[select3.length] = new Option(fileName, fileName);
                var select4 = document.getElementById("seeFileName");
                select4[select4.length] = new Option(fileName, fileName);

            }
        },
        fail: function(error) {
            status.innerHTML = status.innerHTML + "Error creating file" + fileName + "<br>";
        }
    });

    }
    else{
        status.innerHTML = status.innerHTML + "invalid file name<br>";
    }

});

function clearFunction(){

    var status = document.getElementById("statusBox");
    status.innerHTML = "hi";

}

$("#addIndi").submit(function(e){

    var status = document.getElementById("statusBox");
    var GEDCOMtable = document.getElementById("GEDCOMviewTable");
    var fileTable = document.getElementById("fileViewTable");

    e.preventDefault();

    var fileName = document.getElementById("addIndiFileName").value;
   // var source = document.getElementById("source").value;
    var info = { "givenName" : document.getElementById("givn").value,
    "surname" : document.getElementById("surn").value, 
    "sex" : document.getElementById("sex").value,
    };

    for(var j = GEDCOMtable.rows.length -1; j > 0; --j){
        GEDCOMtable.deleteRow(j);
    }
  //  for(var i = 0; i < GEDCOMtable.rows.length -1; ++i){
  //      GEDCOMtable.deleteRow(i +1);
  //  }

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/addIndi',
        data : {
            fileName : fileName,
            info : info,
        },
        success: function (data) {
            if(data.error != "OK"){
                status.innerHTML = status.innerHTML + "Failed to add " + info.givenName + " " + info.surname + "<br>";
            }
            else{
                status.innerHTML = status.innerHTML + "Added " + info.givenName + " " + info.surname + " to " + fileName + "<br>";
                for(var i = 0; i < fileTable.length; ++i){
                    if(fileTable.rows[i].cells[0].innerHTML == fileName){
                        console.log(fileTable.rows[i].cells[6].innerHTML);
                        fileTable.rows[i].cells[6].innerHTML = fileTable.rows[i].cells[6].innerHTML + 1;
                        break;
                    }
                }
            }
            for(var i = 0; i < data.list.length; ++i){
                var row = GEDCOMtable.insertRow(i + 1);
                var givn = row.insertCell(0);
                var surn = row.insertCell(1);
                var sex = row.insertCell(2);
                var famSize = row.insertCell(3);
                givn.innerHTML = data.list[i].givenName;
                surn.innerHTML = data.list[i].surname;
                sex.innerHTML = data.list[i].sex;
                famSize.innerHTML = data.list[i].famSize;
            }
        },
        fail: function(error) {
            status.innerHTML = status.innerHTML + "Failed to add individuals" + "<br>";
        }
    });     
        
});    

$("#getDesc").submit(function(e){

    var status = document.getElementById("statusBox");
    e.preventDefault();

    var fileName = document.getElementById("getDescFile").value;

    var indiGivn = document.getElementById("descFirstName").value;

    var indiSurn = document.getElementById("descSurname").value;

    var numGen = document.getElementById("descNumGen").value;
    if(numGen == undefined){
        numGen = 0;
    }


    var table = document.getElementById("descTable");
    table.innerHTML = "";
    var person = document.getElementById("descLabel");
    person.innerHTML = indiGivn + " " + indiSurn + "'s Descendants:";


    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/desc',
        data : {
            fileName : fileName,
            indiGivn : indiGivn,
            indiSurn : indiSurn,
            numGen : numGen,
        },
        success: function (data) {
            if(data.error != "OK"){
                status.innerHTML = status.innerHTML + "Error getting descendants for " + indiGivn +" " + indiSurn + "<br>";
                var row = table.insertRow(0);
                row.innerHTML = "Error getting descendants";
            }
            else if(data.list.length == 0){
                var row = table.insertRow(0);
                row.innerHTML = "No descendants to display";
            }
            else{
                status.innerHTML = status.innerHTML + "Retrieved descendants for " + indiGivn + " " + indiSurn + "<br>";
                for(i = 0; i < data.list.length; ++i){
                    var row = table.insertRow(i);
                    for(j = 0; j < data.list[i].length; ++j){
                        var cell = row.insertCell(j);
                        cell.innerHTML = data.list[i][j].givenName + " " +  data.list[i][j].surname;
                    }
                }
            }
        },
        fail: function(error) {
            status.innerHTML = status.innerHTML + "Error getting descendants<br>";
        }
    });




});    

$("#getAnce").submit(function(e){

    var status = document.getElementById("statusBox");
    e.preventDefault();

    var fileName = document.getElementById("getAnceFile").value;

    var indiGivn = document.getElementById("anceFirstName").value;

    var indiSurn = document.getElementById("anceSurname").value;

    var numGen = document.getElementById("anceNumGen").value;
    if(numGen == undefined){
        numGen = 0;
    }

    var table = document.getElementById("anceTable");
    table.innerHTML = "";
    var person = document.getElementById("anceLabel");
    person.innerHTML = indiGivn + " " + indiSurn + "'s Ancestors";

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/ance',
        data : {
            fileName : fileName,
            indiGivn : indiGivn,
            indiSurn : indiSurn,
            numGen : numGen,
        },
        success: function (data) {

            if(data.error != "OK"){
                status.innerHTML = status.innerHTML + "Error getting ancestors for " + indiGivn + " " + indiSurn + "<br>";
                var row = table.insertRow(0);
                row.innerHTML = "Error getting ancestors";
            }
            else if(data.list.length == 0){
                var row = table.insertRow(0);
                row.innerHTML = "No ancestors to display";
            }
            else{
                status.innerHTML = status.innerHTML + "Retrieved ancestors for " + indiGivn + " " + indiSurn + "<br>";
                for(i = 0; i < data.list.length; ++i){
                    var row = table.insertRow(i);
                    for(j = 0; j < data.list[i].length; ++j){
                        var cell = row.insertCell(j);
                        cell.innerHTML = data.list[i][j].givenName + " " +  data.list[i][j].surname;
                    }
                }
            }
            console.log("here");
        },
        fail: function(error) {
            status.innerHTML = status.innerHTML + "Error getting ancestors<br>";
        }
    });

}); 


$("#login").submit(function(e){

    var database = document.getElementById("databaseName").value;

    var username = document.getElementById("username").value;

    var password = document.getElementById("password").value;

    e.preventDefault();

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type = we will use JSON for almost everything 
        url: '/login',
        data : {
            database : database,
            username : username,
            password : password,
        },
        success: function (data) {
            console.log("hey paige");
            var login = document.getElementById("loginError");
            if(data.error == "none"){
                login.innerHTML = "Successful Login";
                var db = document.getElementById("databaseStuff");
                db.style.visibility = "visible";
            }
            else{
                login.innerHTML = "Invalid username/passwords/database";
            }
        },
        fail: function(error) {
            console.log("hello????");
            var loginErr = document.getElementById("loginError");
            loginErr.innerHTML = "Invalid username/password";
        }
    });
});

function storeData(){

    var status = document.getElementById("statusBox");

    console.log("in store data");

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/storeData',
        success: function(data) {
            console.log("o nice");
            status.innerHTML = status.innerHTML + "All data stored to database<br>";

            for(let file of data){
                var select = document.getElementById("getIndisDBfileName");
                select[select.length] = new Option(file.fileName, file.fileName);
            }

        },
        fail: function(error) {
            console.log("o jeez");
        }
    });

};

function clearData(){

    var status = document.getElementById("statusBox");

    console.log("in clear data");

    $.ajax({
        type: 'get',
        url: '/clearData',
        success: function(data) {
            console.log("oooo boy");
            if(data.error == "none"){
                status.innerHTML = status.innerHTML + "All data cleared from database<br>";
            }
            else{
                status.innerHTML = status.innerHTML + "Failed to clear data<br>";
            }
        },
        fail: function(error){
            console.log("oo man");
        }
    });

};

function displayDB(){

    var status = document.getElementById("statusBox");

    console.log("in display DB status");

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/displayDBfiles',
        success: function(data) {
            status.innerHTML = status.innerHTML + "Database has " + data.files + " files<br>";
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/displayDBinds',
        success: function(data) {
            status.innerHTML = status.innerHTML + "Database has " + data.inds + " individuals<br>";
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });

};

function dbLastName(){

    var table = document.getElementById("databaseTable");
    table.innerHTML = "";

    var row1 = table.insertRow(0);

    var givenName1 = row1.insertCell(0);
    var surname1 = row1.insertCell(1);
    var sex1 = row1.insertCell(2);
    var famSize1 = row1.insertCell(3);

    givenName1.innerHTML = "Given Name";
    surname1.innerHTML = "Surname";
    sex1.innerHTML = "Sex";
    famSize1.innerHTML = "Family Size";

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/displayByLastName',
        success: function(data) {
            var i = 1;
            for(let line of data){
                var row = table.insertRow(i);
                var givenName = row.insertCell(0);
                var surname = row.insertCell(1);
                var sex = row.insertCell(2);
                var famSize = row.insertCell(3);

                givenName.innerHTML = line.given_name;
                surname.innerHTML = line.surname;
                sex.innerHTML = line.sex;
                famSize.innerHTML = line.fam_size;
                ++i;
            }
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });

}

$("#getIndisByDBfile").submit(function(e){

    e.preventDefault();

    var fileName = document.getElementById("getIndisDBfileName").value;

    var table = document.getElementById("databaseTable");
    table.innerHTML = "";

    var row1 = table.insertRow(0);

    var givenName1 = row1.insertCell(0);
    var surname1 = row1.insertCell(1);
    var sex1 = row1.insertCell(2);
    var famSize1 = row1.insertCell(3);

    givenName1.innerHTML = "Given Name";
    surname1.innerHTML = "Surname";
    sex1.innerHTML = "Sex";
    famSize1.innerHTML = "Family Size";

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/indisByFile',
        data : {
            fileName : fileName,
        },
        success: function(data) {
            if(data.err == "yes"){
                var row = table.inserRow(1);
                row.innerHTML = "Error getting data";
            }
            else{
                var i =1;
                for(let line of data){
                    var row = table.insertRow(i);
                    var givenName = row.insertCell(0);
                    var surname = row.insertCell(1);
                    var sex = row.insertCell(2);
                    var famSize = row.insertCell(3);

                    givenName.innerHTML = line.given_name;
                    surname.innerHTML = line.surname;
                    sex.innerHTML = line.sex;
                    famSize.innerHTML = line.fam_size;
                    ++i;
                }
            }
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });

});

$("#anyQuery").submit(function(e){

    e.preventDefault();

    var que = document.getElementById("anyQueryText").value;

    console.log('hi');

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/anyQuery',
        data : {
            que : que,
        },
        success : function(data) {
            if(data.err == "yes"){
                var table = document.getElementById("databaseTable");
                table.innerHTML = "";

                var row = table.insertRow(0);
                row.innerHTML = "Error getting data";
            }
            else{
                makeTable(data.data);
            }
        },
        fail : function(error){

        }


    });

});

function closeWindow(){

    var hi = document.getElementById("loginWindow");
    hi.style.display = "none";

}

function helpWindow(){

    var hi = document.getElementById("helpWindow");
    hi.style.display = "block";

    var fileTable = document.getElementById("FILEhelp");
    var indTable = document.getElementById("INDIVIDUALhelp");

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/helpFILE',
        success: function(data) {
            if(data.err == "yes"){
                var row = fileTable.insertRow(0);
                row.innerHTML = "Error";
            }
            else{
                var json = data.data[0];
                var row = fileTable.insertRow(0);
                var i = 0;
                for( key in json){
                    var cell = row.insertCell(i);
                    cell.innerHTML = key;
                    ++i;
                }

                i = 1;
                for(let line of data.data){
                    var row = fileTable.insertRow(i);
                    var j = 0;
                    for(key in json){
                        var cell = row.insertCell(j);
                        cell.innerHTML = line[key];
                        ++j;
                    }
                    ++i;
                }
            }
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/helpIND',
        success: function(data) {
            if(data.err == "yes"){
                var row = indTable.insertRow(0);
                row.innerHTML = "Error";
            }
            else{
                var json = data.data[0];
                var row = indTable.insertRow(0);
                var i = 0;
                for( key in json){
                    var cell = row.insertCell(i);
                    cell.innerHTML = key;
                    ++i;
                }

                i = 1;
                for(let line of data.data){
                    var row = indTable.insertRow(i);
                    var j = 0;
                    for(key in json){
                        var cell = row.insertCell(j);
                        cell.innerHTML = line[key];
                        ++j;
                    }
                    ++i;
                }
            }
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });


}

function closeHelp(){

    var hi = document.getElementById("helpWindow");
    hi.style.display = "none";

}

function makeTable(data){

    var json = data[0];

    var table = document.getElementById("databaseTable");
    table.innerHTML = "";
    var row = table.insertRow(0);
    var i = 0;
    for( key in json){
        var cell = row.insertCell(i);
        cell.innerHTML = key;
        ++i;
    }

    i = 1;
    for(let line of data){
        var row = table.insertRow(i);
        var j = 0;
        for(key in json){
            var cell = row.insertCell(j);
            cell.innerHTML = line[key];
            ++j;
        }
        ++i;
    }

}

$("#bySurname").submit(function(e){

    e.preventDefault();

    var surname = document.getElementById("surnameText").value;
    var sex = document.getElementById("surnameSex").value;

    var table = document.getElementById("databaseTable");
    table.innerHTML = "";

    var row1 = table.insertRow(0);

    var givenName1 = row1.insertCell(0);
    var surname1 = row1.insertCell(1);
    var sex1 = row1.insertCell(2);
    var famSize1 = row1.insertCell(3);

    givenName1.innerHTML = "Given Name";
    surname1.innerHTML = "Surname";
    sex1.innerHTML = "Sex";
    famSize1.innerHTML = "Family Size";

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/displayOfSurname',
        data : {
            surname : surname,
            sex : sex
        },
        success : function(data) {
            if(data.err == "yes"){
                var row = table.inserRow(1);
                row.innerHTML = "Error getting data";
            }
            else{
                var i =1;
                for(let line of data){
                    var row = table.insertRow(i);
                    var givenName = row.insertCell(0);
                    var surname = row.insertCell(1);
                    var sex = row.insertCell(2);
                    var famSize = row.insertCell(3);

                    givenName.innerHTML = line.given_name;
                    surname.innerHTML = line.surname;
                    sex.innerHTML = line.sex;
                    famSize.innerHTML = line.fam_size;
                    ++i;
                }
            }
        },
        fail : function(error){
            console.log("a failure : (");
        }


    });

});

function avgNumIndi(){

    var table = document.getElementById("databaseTable");
    table.innerHTML = "";

    var row = table.insertRow(0);

    row.innerHTML = "Average Number of individual Per File";

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/avgNumIndi',
        success: function(data) {
            row = table.insertRow(1);
            if(data.err == "yes"){
                row.innerHTML = "Error getting average";
            }
            else{
                row.innerHTML = data.data[0].hello;
            }
        },
        fail: function(error){
            console.log("a failure :,(");
        }
    });
    
}

function indiWithFile(){

    var table = document.getElementById("databaseTable");
    table.innerHTML = "";

    console.log("hi");

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/indiWithFile',
        success : function(data) {
            if(data.err == "yes"){
                var table = document.getElementById("databaseTable");
                table.innerHTML = "";

                var row = table.inserRow(0);
                row.innerHTML = "Error getting data";
            }
            else{
                makeTable(data.data);
            }
        },
        fail : function(error){

            console.log("a failure : (");
        }


    });
}