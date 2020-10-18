from azure.storage.file import FileService
service_client = ShareServiceClient.from_connection_string(connection_string)
file_service.get_file_to_path('mhack456738', None, 'hack_detector.h5', 'out-detctor.h5')
generator = file_service.list_directories_and_files('mhackamolly')
for file_or_dir in generator:
    print(file_or_dir.name)
with open(FILE_PATH, 'wb') as stream:
    file = file_service.get_file_to_stream(
        share_name, directory_name, file_name, stream, snapshot=snapshot_id