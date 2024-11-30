# ShapeLibForESP32
- A library to read/write ESRI Shapefiles.
- ESP32 port of [OSGEO/shapelib](https://github.com/OSGeo/shapelib) (from the commit [36106c2](https://github.com/OSGeo/shapelib/commit/36106c2b80ab253f022d756f0ecdd2f952620d56)).
- WIP. We need testers. So far, it has only been tested on ESP32-S3 for polygon readings.
- SD library is used for storing and reading from SHP files.
- Even though the original C sources were converted to C++ for easy interfacing with the SD library, API is still C-ish.

## Why?
- There are so many possible scenarios. For instance, you create polygons on QGIS or ArcGIS, and you can also assign values to the attribute table corresponding to your regions (polygons). Assume your board is equipped with a GPS receiver. Do something in polygon A and do another thing in polygon B, etc.

## Example code
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

  SHPHandle hSHP = SHPOpen("/bolgeler/bolgeler.shp", "rb");
  DBFHandle hDBF = DBFOpen("/bolgeler/bolgeler.dbf", "rb");
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