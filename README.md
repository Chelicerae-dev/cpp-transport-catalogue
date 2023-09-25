# cpp-transport-catalogue

Console app - transport catalogue, used for storing and processing information about bus routes. Input and output data format is JSON.
Functions are calculating shortest route between 2 stops, visualization of whole bus routes' map in SVG format, retrieving bus or stop data.
App implements serialization/deserialization using Google Protocol Buffers for quick result calculation after creating database.

TODO: GUI (using Qt).

Build
For building project use CMake and file /transport-catalogue/CMakeLists.txt
Protobuf (https://protobuf.dev/) required. Path to Protobuf set in 8th line of CMakeLists.txt
C++ 17. CMake 3.10 and higher.

Use
Run software with either flag:
  make_base - database creation and serialization;
  process_requests - database deserialization and handling of user requests.

Input data format (JSON):
  
    make_base:
      serialization_settings - dictionary with "file" pair. Value of "file" contains path to file, where database will be saved after serialization;
      base_requests - array of objects, representing stops and buses info:
        type - string. "Stop" or "Bus";
        name - string. Contains name;
        Type-specific fileds:
          Bus:
            is_roundtrip - Bus-specific field, bool. 1 -> 2 -> 3 -> 1... is roundtrip, 1 -> 2 -> 3 -> 2 -> 1 isn't;
            stops - Bus-specific, array. Contains list of stop names;
          Stop:
            latitude, longitude - Stop-specific fields, double. Contain latitude and longitude values respectively;
            road_distances - Stop-specific, dictionary. Contains pairs "Stop name" : distance (int) with distances from current stop to "Stop name" in meters;

      render_settings - dictionary, contains render settings for SVG visualization of bus routes' map. Fields:
        width, height - ints. Contain canvas size for map visualization in pixels
        padding - int, contains padding value in pixels;
        stop_radius - int, contains stop symbol radius value in pixels;
        line_width - int, contains line width value in pixels;
        bus_label_font_size - int, contains bus label font size in pixels;
        bus_label_offset - array [int, int], contains x and y values for bus label offset in pixels;
        stop_label_font_size, stop_label_offset - same as above, but for stop labels instead of buses';
        underlayer_color - one of following: string, rgb, rgba object (array), contains color value of text underlayers;
        underlayer_width - int, contains text underlayer width value in pixels;
        color_palette - array of color objects (any of following: string, "none", rgb or rgba object (array)). Contains palette colors list, used repetitatively for bus (lines) visualization;
  
      routing_settings - dictionary, contains settings for route calculation. Fields: 
        bus_velocity - int, bus velocity value in km/h
        bus_wait_time - int, bus wait time on each stop in munutes

    process_requests:
      serialization_settings - dictionary with "file" pair. Value of "file" contains path to database file;
      stat_requests - array of objects, representing database requests:
        id - int, unique id of request;
        type - string, possible values are "Stop", "Bus", "Map", "Route";
        name - string, contains name of "Stop" or "Bus". Map and Route requests don't have this field;
        from, to - strings, used only for "Route" type requests. Contain names of stops, representing start and end points of route respectively.


