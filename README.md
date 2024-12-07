# ShapeLibForESP32
- A library to read/write ESRI Shapefiles.
- ESP32 port of [OSGEO/shapelib](https://github.com/OSGeo/shapelib) (from the commit [36106c2](https://github.com/OSGeo/shapelib/commit/36106c2b80ab253f022d756f0ecdd2f952620d56)).
- WIP. We need testers. So far, it has only been tested on ESP32-S2 and ESP32-S3.
- SD library is used for storing and reading from SHP files.
- Even though the original C sources were converted to C++ for easy interfacing with the SD library, API is still C-ish.

## Why?
- There are so many possible scenarios. For instance, you create polygons on QGIS or ArcGIS. Assume your board is equipped with a GPS receiver. You can program your board to achieve tasks based-on the GPS position data. Do something in polygon A and do another thing in polygon B, etc. Or save shapefiles on your SD card from your GPS receiver. 


## Example code 1
The example here contains 8 polygons and two data fields in the attribute table.
- Read shape and dbf files from SD card.
- Read and display info
- List lat and lon for polygon vertices
- List values of the attribute table

```c++
#include "shapefil.hpp"
#include "SD.h"

void setup() {
  Serial.begin(115200);

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  SHPsetFileSystem(SD);

  SHPHandle hSHP = SHPOpen("/bolgeler/bolgeler.shp", "r");
  DBFHandle hDBF = DBFOpen("/bolgeler/bolgeler.dbf", "r");
  if (hSHP == NULL || hDBF == NULL) {
    Serial.println("Failed to open shapefile or DBF file!");
    return;
  }

  // Get the number of entities and the type in the SHP file
  int nEntitiesSHP, nShapeType;
  SHPGetInfo(hSHP, &nEntitiesSHP, &nShapeType, NULL, NULL);
  Serial.printf("Shapefile: %d entities, Type: %d\n", nEntitiesSHP, nShapeType);

  // Get the number of records in the DBF file
  int nEntitiesDBF = DBFGetRecordCount(hDBF);
  Serial.printf("DBF file: %d records\n", nEntitiesDBF);

  // List field names
  int nFields = DBFGetFieldCount(hDBF);
  int iWidth, iDecimals;
  Serial.println("Field names:");
  for (int iField = 0; iField < nFields; ++iField) {
    char szFieldName[12];
    DBFGetFieldInfo(hDBF, iField, szFieldName, &iWidth, &iDecimals);
    Serial.printf("  Field %d: %s\n", iField, szFieldName);
  }

  // Read data from both files
  for (int iEntity = 0; iEntity < nEntitiesSHP; ++iEntity) {
    SHPObject* shp = SHPReadObject(hSHP, iEntity);
    Serial.printf("Entity %d has %d vertices:\n", iEntity, shp->nVertices);

    for (int iVertex = 0; iVertex < shp->nVertices; ++iVertex) {
      Serial.printf("  lon: %f, lat: %f\n", shp->padfX[iVertex], shp->padfY[iVertex]);
    }

    Serial.println("Attributes:");
    for (int iField = 0; iField < DBFGetFieldCount(hDBF); ++iField) {
      const char* value = DBFReadStringAttribute(hDBF, iEntity, iField);
      Serial.printf("  Field %d: %s\n", iField, value ? value : "NULL");
    }

    SHPDestroyObject(shp);
  }

  SHPClose(hSHP);
  DBFClose(hDBF);
}

void loop() {}

```

## Example code 2
This example demonstrates working with a Shapefile and a corresponding DBF file stored on the SD card:

- Create or open a shapefile and corresponding DBF file on the SD card.
- Write a polygon (with 4 vertices) to the shapefile.
- Create a test field in the DBF file and write a value to the attribute table.
- Read and display the shapefile's geometry and attribute data.
- Display the polygon vertices' latitude and longitude (coordinates).
- Write test values to the attribute table.

```c++
#include "shapefil.hpp"
#include "SD.h"

void setup() {
  Serial.begin(115200);
  
  delay(1000);

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  SHPsetFileSystem(SD);
  
  const char *filename = "/p1.shp";
  const char *dbfFilename = "/p1.dbf";

  File file = SD.open(filename);
  if(!file) {
    file.close();
    Serial.println("File doesn't exist. Let's create it first!");
    Serial.println("Creating file...");
    SHPHandle hSHP = SHPCreate(filename, SHPT_POLYGON);

    if (hSHP == nullptr) {
      Serial.println("Failed to create SHP file!");
      return;
    } else {
      Serial.println("Shapefile created successfully!");
    }

    SHPClose(hSHP);

    // Create corresponding DBF file
    DBFHandle hDBF = DBFCreate(dbfFilename);
    if (hDBF == nullptr) {
      Serial.println("Failed to create DBF file!");
      return;
    } else {
      Serial.println("DBF file created successfully!");
    }

    // Add a test field to the DBF file
    if (DBFAddField(hDBF, "TestField", FTString, 20, 0) == -1) {
      Serial.println("Failed to add field to DBF file!");
      DBFClose(hDBF);
      return;
    }

    DBFClose(hDBF);
  } else {
    file.close();
    Serial.println("Using existing files...");
  }

  delay(1000);
  
  SHPHandle hSHP = SHPOpen(filename, "r+");
  DBFHandle hDBF = DBFOpen(dbfFilename, "r+");
  if (hSHP == nullptr || hDBF == nullptr) {
      Serial.println("Failed to open SHP or DBF file!");
      return;
  }

  int nShapeType;
  SHPGetInfo(hSHP, NULL, &nShapeType, NULL, NULL);
  Serial.println(nShapeType);

  // If the file is empty, set a type
  if (nShapeType == SHPT_NULL) {
      nShapeType = SHPT_POLYGON;
  }

  /* -------------------------------------------------------------------- */
  /*      Define example data.                                            */
  /* -------------------------------------------------------------------- */
  const int nVertices = 4; // Number of vertices
  double padfX[4] = {0.0, 10.0, 10.0, 0.0}; // X coordinates
  double padfY[4] = {0.0, 0.0, 10.0, 10.0}; // Y coordinates
  int panParts[1] = {0}; // Part start positions (for single-part polygon)

  /* -------------------------------------------------------------------- */
  /*      Create Shapefile object.                                        */
  /* -------------------------------------------------------------------- */
  SHPObject *psObject = SHPCreateObject(
      nShapeType, -1, 1, panParts, NULL, nVertices, padfX, padfY, NULL, NULL);
  if (psObject == nullptr) {
      Serial.printf("Failed to create Shapefile object.\n");
      SHPClose(hSHP);
      DBFClose(hDBF);
      return;
  }

  /* -------------------------------------------------------------------- */
  /*      Write to the Shapefile.                                         */
  /* -------------------------------------------------------------------- */
  Serial.println("Writing to disk...");
  if (SHPWriteObject(hSHP, -1, psObject) < 0) {
      Serial.printf("Failed to write to Shapefile.\n");
      SHPDestroyObject(psObject);
      SHPClose(hSHP);
      DBFClose(hDBF);
      return;
  }

  /* -------------------------------------------------------------------- */
  /*      Write data to the attribute table.                              */
  /* -------------------------------------------------------------------- */
  const char *testValue = "TestValue";
  if (!DBFWriteStringAttribute(hDBF, 0, 0, testValue)) { // Write to the 0th record
      Serial.println("Failed to write attribute to DBF file!");
  } else {
      Serial.println("Attribute written successfully!");
  }

  /* -------------------------------------------------------------------- */
  /*      Cleanup and exit.                                               */
  /* -------------------------------------------------------------------- */
  SHPDestroyObject(psObject);
  SHPClose(hSHP);
  DBFClose(hDBF);

  Serial.printf("Shapefile and DBF successfully written: %s\n", filename);
}

void loop() {}

```