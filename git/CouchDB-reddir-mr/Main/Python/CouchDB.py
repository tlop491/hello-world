#!/usr/bin/python
import time
import os
import uuid
import webbrowser

urlLocal = "http://localhost:5984/_utils/"

urlAdmin = "http://admin:123456@127.0.0.1:5984/"

def openWeb(url):
    webbrowser.open( url, new=2)

#Delete Db on start 
def deleteDB( url, nameDB  ): 
	os.system("curl -vX DELETE " + url  + nameDB )  

def createDB(url, nameDB ): 
	#Create a Database called albums backup 
	os.system("curl -vX PUT "  + url + nameDB)


def createDBDoc (url, nameDB, data): 
	#Generate UUDI
	tmpID = uuid.uuid4().hex
	os.system("curl -X PUT " + url + nameDB + "/" + str(tmpID) + " -d " + data )	
	 

def checkDB (url, nameDB): 
    os.system("curl -X GET "  + url + nameDB)



def loadJson (url, nameDB, json):
    os.system("cat " + json + "| POST -sS \"" + url + nameDB + "/_bulk_docs\" -c \"application/json\"" )


def samplePopulate (num, url, nameDB, json):
    for i in range(1,num):
        print i
        loadJson( url, nameDB, json)



def putView(viewFile, url , nameDB, view): 
    #Y no work 
    os.system("curl -X PUT viewFile" +  url  + nameDB + "/_design/" + view)     



def getView(url, nameDB, view):
    os.system("curl -X GET \'" + url + "/" + nameDB + "/_design/docs/_view/" + view + "\'") 
    

def getViewInfo(url, nameDB, view):
    os.system("curl -X GET \'" + url + "/" + nameDB + "/_design/docs/\'") 


openWeb( urlLocal)

#Create/Delete Database

#deleteDB(urlAdmin, "reddit")

createDB(urlAdmin, "reddit")
#createDB(urlAdmin, "reddit200mb")
#checkDB( urlAdmin, "reddit1")
#createDB(urlAdmin, "reddit300mb")

#createDB(urlAdmin, "reddit400mb")


samplePopulate ( 5000,  urlAdmin, "reddit", "samplemedium.json")
#samplePopulate( 500, urlAdmin, "reddit100mb", "samplemedium.json")
#samplePopulate( 1000, urlAdmin, "reddit200mb", "samplemedium.json")
#samplePopulate( 1500, urlAdmin, "reddit300mb", "samplemedium.json")
#putView( viw.js, urlAdmin, reddit, findmax)

#getViewInfo(urlAdmin, "reddit", "findmax")
#start = time.time()
#getView(urlAdmin, "reddit200mb", "200map")
#end = time.time()
#print(end - start)
