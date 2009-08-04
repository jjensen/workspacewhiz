#ifndef VALIDATEREGISTRATIONCODE_H
#define VALIDATEREGISTRATIONCODE_H

bool ValidateRegistrationCode(const char* registrationCode, const char* registrationName,
								time_t& registrationTime);

#endif // VALIDATEREGISTRATIONCODE_H