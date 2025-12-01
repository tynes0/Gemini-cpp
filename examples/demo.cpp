#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

#include "gemini/client.h"
#include "gemini/logger.h"
#include "gemini/request_builder.h"
#include "gemini/chat_session.h"

#ifdef _WIN32
#include <windows.h>
#endif

void print_separator(const std::string& title)
{
    std::cout << "\n\033[1;35m========================================\033[0m\n";
    std::cout << "\033[1;36m " << title << " \033[0m\n";
    std::cout << "\033[1;35m========================================\033[0m\n";
}

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    // 1. Loglama Başlatılıyor
    GeminiCPP::Log::init();
    // Log seviyesini her şeyi görecek şekilde ayarlayalım (varsayılan pattern zaten ayarlı)
    
    GEMINI_INFO("Gemini C++ Client Demo Başlatılıyor...");

    // 2. API Key Kontrolü
    std::string apiKey = std::getenv("GEMINI_API_KEY") ? std::getenv("GEMINI_API_KEY") : "";
    if(apiKey.empty())
    {
        GEMINI_CRITICAL("HATA: GEMINI_API_KEY çevre değişkeni bulunamadı!");
        return 1;
    }

    GeminiCPP::Client client(apiKey);

    // --- TEST 1: API Anahtarı Doğrulama ---
    print_separator("TEST 1: API Key & Bağlantı Kontrolü");
    auto verifyRes = client.verifyApiKey();
    if(verifyRes.isValid)
    {
        GEMINI_INFO("Bağlantı Başarılı! Durum Kodu: {}", (int)verifyRes.statusCode);
    }
    else
    {
        GEMINI_ERROR("Bağlantı Başarısız: {}", verifyRes.message);
        return 1;
    }

    // --- TEST 2: Modelleri Listeleme (Models API) ---
    print_separator("TEST 2: Models API - Listeleme ve Getirme");
    
    GEMINI_TRACE("Modeller listeleniyor...");
    auto modelsRes = client.models.list();
    
    if(modelsRes.success)
    {
        GEMINI_INFO("Toplam {} model bulundu.", modelsRes->models.size());
        if(!modelsRes->models.empty())
        {
            // İlk 3 modeli yazdıralım
            for(size_t i = 0; i < (std::min)((size_t)3, modelsRes->models.size()); ++i)
            {
                const auto& m = modelsRes->models[i];
                GEMINI_DEBUG("Model [{}]: {} ({})", i, m.displayName, m.name.str());
            }
        }

        // Spesifik bir modelin detayını çekelim (örn: gemini-1.5-flash)
        GEMINI_TRACE("gemini-1.5-flash detayları çekiliyor...");
        auto modelInfo = client.models.get("models/gemini-2.5-flash");
        if(modelInfo.success)
        {
            GEMINI_INFO("Model Detayı: {} - Max Input Token: {}", modelInfo->displayName, modelInfo->inputTokenLimit);
        }
        else
        {
            GEMINI_WARN("Model detayı çekilemedi: {}", modelInfo.errorMessage);
        }
    }
    else
    {
        GEMINI_ERROR("Modeller listelenemedi: {}", modelsRes.errorMessage);
    }

    // --- TEST 3: Token Sayma (Tokens API) ---
    print_separator("TEST 3: Tokens API - Token Sayma");
    
    std::string sampleText = "Merhaba Gemini, bu bir token sayma testidir. C++ harika bir dildir.";
    GEMINI_TRACE("Metin için token sayılıyor: '{}'", sampleText);
    
    auto tokenRes = client.tokens.count("models/gemini-2.5-flash", sampleText);
    if(tokenRes.success)
    {
        GEMINI_INFO("Hesaplanan Token Sayısı: {}", tokenRes->totalTokens);
    }
    else
    {
        GEMINI_ERROR("Token sayma başarısız: {}", tokenRes.errorMessage);
    }

    // --- TEST 4: İçerik Üretimi (Generate Content - Sync) ---
    print_separator("TEST 4: Generate Content (Senkron)");
    
    GEMINI_DEBUG("İstek oluşturuluyor (Model: gemini-2.0-flash)...");
    auto response = client.request()
        .model(GeminiCPP::Model::GEMINI_2_0_FLASH)
        .text("Bana C++ programlama dili hakkında kısa, motive edici tek bir cümle söyle.")
        .temperature(0.7f)
        .generate();

    if(response.success)
    {
        GEMINI_INFO("Gemini Cevabı: \033[1;32m{}\033[0m", response.text());
        GEMINI_DEBUG("Kullanılan Token: Giriş={}, Çıkış={}", response.inputTokens, response.outputTokens);
    }
    else
    {
        GEMINI_ERROR("Üretim hatası: {}", response.errorMessage);
    }

    // --- TEST 5: İçerik Üretimi (Stream) ---
    print_separator("TEST 5: Generate Content (Stream - Akış)");
    
    GEMINI_DEBUG("Stream isteği gönderiliyor...");
    std::cout << "\033[1;33mStream Çıktısı: \033[0m"; // Sarı renkli başlık
    
    auto streamRes = client.request()
        .model(GeminiCPP::Model::GEMINI_2_0_FLASH)
        .text("1'den 5'e kadar yavaşça say ve her sayının yanına bir meyve ismi ekle.")
        .stream([](std::string_view chunk) {
            // Callback: Gelen parçayı ekrana bas
            std::cout << chunk << std::flush;
        });
    
    std::cout << "\n"; // Stream bitti, alt satıra geç
    
    if(streamRes.success)
    {
        GEMINI_INFO("Stream tamamlandı.");
    }
    else
    {
        GEMINI_ERROR("Stream hatası: {}", streamRes.errorMessage);
    }

    // --- TEST 6: Sohbet Oturumu (Chat Session) ---
    print_separator("TEST 6: Chat Session (Hafızalı Sohbet)");
    
    auto chat = client.startChat(GeminiCPP::Model::GEMINI_2_0_FLASH);
    
    // Mesaj 1
    GEMINI_TRACE("Kullanıcı: Merhaba, adım Tynes0.");
    auto chatResp1 = chat.send("Merhaba, benim adım Tynes0. Ben bir C++ geliştiricisiyim.");
    GEMINI_INFO("Gemini: {}", chatResp1.text());

    // Mesaj 2 (Hafıza Testi)
    GEMINI_TRACE("Kullanıcı: Benim adım ne ve ne iş yapıyorum?");
    auto chatResp2 = chat.send("Sana az önce adımı ve mesleğimi söyledim, hatırlıyor musun?");
    
    if(chatResp2.success)
    {
        GEMINI_INFO("Gemini: {}", chatResp2.text());
    }
    else
    {
        GEMINI_ERROR("Chat hatası: {}", chatResp2.errorMessage);
    }

    // --- TEST 7: Files API (Yükleme ve Silme) ---
    print_separator("TEST 7: Files API (Dosya Yönetimi)");

    // Geçici dosya oluşturma
    std::string tempFileName = "test_gemini_cpp.txt";
    {
        std::ofstream tempFile(tempFileName);
        tempFile << "Bu dosya Gemini C++ kütüphanesi tarafından test amaçlı oluşturulmuştur.";
        tempFile.close();
    }
    GEMINI_DEBUG("Geçici dosya oluşturuldu: {}", tempFileName);

    // 1. Upload
    GEMINI_TRACE("Dosya yükleniyor...");
    auto uploadRes = client.files.upload(tempFileName, "GeminiCPP Test File");
    
    if(uploadRes.success)
    {
        std::string fileUri = uploadRes->uri;
        std::string fileName = uploadRes->name.str();
        GEMINI_INFO("Dosya Yüklendi! URI: {}", fileUri);
        GEMINI_DEBUG("Sunucu Dosya Adı (ID): {}", fileName);

        // 2. Get Metadata
        GEMINI_TRACE("Dosya metadatası sorgulanıyor...");
        auto getFileRes = client.files.get(fileName);
        if(getFileRes.success)
        {
            GEMINI_INFO("Dosya Durumu: {}", frenum::to_string(getFileRes->state));
        }

        // 3. Delete
        GEMINI_TRACE("Dosya siliniyor...");
        auto deleteRes = client.files.deleteFile(fileName);
        if(deleteRes.success)
        {
            GEMINI_INFO("Dosya başarıyla silindi.");
        }
        else
        {
            GEMINI_ERROR("Silme hatası: {}", deleteRes.errorMessage);
        }
    }
    else
    {
        GEMINI_ERROR("Yükleme başarısız: {}", uploadRes.errorMessage);
    }

    // Geçici dosyayı temizle
    std::filesystem::remove(tempFileName);

    // --- TEST 8: Async İşlemler ---
    print_separator("TEST 8: Async Generation (Future/Await)");
    
    GEMINI_DEBUG("Async istek başlatılıyor...");
    auto futureRes = client.request()
        .text("Asenkron programlamanın faydası nedir? Tek cümle.")
        .generateAsync();

    // Burada başka işler yapılabilir...
    GEMINI_TRACE("İstek arka planda işlenirken ben buradayım...");

    // Sonucu bekle
    auto asyncResult = futureRes.get();
    if(asyncResult.success)
    {
        GEMINI_INFO("Async Cevap: {}", asyncResult.text());
    }
    else
    {
        GEMINI_ERROR("Async hata: {}", asyncResult.errorMessage);
    }

    print_separator("TESTLER TAMAMLANDI");
    return 0;
}