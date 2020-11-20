package ru.cleverhause.auth.config;

import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Component;

import javax.servlet.http.HttpServletRequest;

@Component
@RequiredArgsConstructor
public class SimpleAuthenticationProviderInfoResolver implements AuthenticationProviderInfoResolver {
    private static final String PROVIDER_TYPE = "type";

    private final ExistentProvidersMap providersMap;

    public ExistentProvidersMap.AuthenticationProviderInfo resolveProvider(HttpServletRequest request) {
        String type = request.getParameter(PROVIDER_TYPE);
        return providersMap.getByType(type);
    }
}
