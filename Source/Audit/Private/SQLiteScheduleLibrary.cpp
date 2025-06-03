#include "SQLiteScheduleLibrary.h"
#include "sqlite3.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

TArray<FClassroomScheduleRow> USQLiteScheduleLibrary::GetScheduleForRoom(const FString& RoomCode)
{
    TArray<FClassroomScheduleRow> Results;

    FString DBPath = FPaths::ProjectPersistentDownloadDir() + "TestDB.db";
    sqlite3* DB;
    if (sqlite3_open(TCHAR_TO_UTF8(*DBPath), &DB) != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Не удалось открыть базу данных: %s"), *DBPath);
        return Results;
    }

    FString Query = TEXT("SELECT subject, teacher, start_time, end_time, weekday FROM ClassroomSchedule WHERE room_code = ?;");
    sqlite3_stmt* Statement = nullptr;

    if (sqlite3_prepare_v2(DB, TCHAR_TO_UTF8(*Query), -1, &Statement, nullptr) == SQLITE_OK)
    {
        // Привязываем RoomCode как параметр
        sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*RoomCode), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(Statement) == SQLITE_ROW)
        {
            FClassroomScheduleRow Row;
            Row.Subject   = UTF8_TO_TCHAR((const char*)sqlite3_column_text(Statement, 0));
            Row.Teacher   = UTF8_TO_TCHAR((const char*)sqlite3_column_text(Statement, 1));
            Row.StartTime = UTF8_TO_TCHAR((const char*)sqlite3_column_text(Statement, 2));
            Row.EndTime   = UTF8_TO_TCHAR((const char*)sqlite3_column_text(Statement, 3));
            Row.Weekday   = UTF8_TO_TCHAR((const char*)sqlite3_column_text(Statement, 4));
            Results.Add(Row);
        }
    }


    sqlite3_finalize(Statement);
    sqlite3_close(DB);

    return Results;
}

